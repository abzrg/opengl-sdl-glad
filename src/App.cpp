#include <iostream>
#include <vector>

#include "SDL2/SDL.h"
#include "glad/glad.h"

#include "App/App.h"

namespace App {

struct NormalizedColor
{
    GLfloat r{};
    GLfloat g{};
    GLfloat b{};
    GLfloat a{};
};

constexpr NormalizedColor bg = {0.0F, 0.0F, 1.F, 1.0F};

constexpr int screenHeight = 480;
constexpr int screenWidth = 640;

SDL_Window *graphicsApplicationWindow = nullptr; // NOLINT
SDL_GLContext openGLContext = nullptr;           // NOLINT

// Vertex Array Object -- VAO
// It encapsulates all of the items needed to render an object. e.g. we may have multiple vertex
// buffer objects related to rendering one object. The VAO allows us to setup the OpenGL state  to
// render the object using the correct layout and correct buffers with one call after being setup.
GLuint vertexArrayObject = 0; // NOLINT

// Vertex Buffer Object -- VBO
// It stores the information relating to vertices (e.g. position, normals, texture). VBOs are our
// mechanism for arranging geometry on the GPU.
GLuint vertexBufferObject = 0; // NOLINT

// Shader program object
// This object stores a unique id for the graphic pipeline program object that will be used for our
// OpenGL draw calls
GLuint graphicsPipelineShaderProgram = 0; // NOLINT

/* At a minimum, every Modern OpenGL program needs a vertex and fragment shader
   OpenGL provides functions that will compile the shader source code (stored as strings) at
   run-time. */

// Vertex Shader
// It executes once per vertex, and will be in charge of the final position of the vertex
/* Homogeneous coordinates:
    A three-dimensional euclidean space point (x, y, z) becomes the homogeneous vertex with
    coordinates (x, y, z, 1.0), and the two-dimensional euclidean point (x, y) becomes (x, y,
    0.0, 1.0). As long as w is nonzero, the homogeneous vertex (x, y, z, w) corresponds to the
    three-dimensional point (x/w, y/w, z/w). */
std::string const vertexShaderSource = /*NOLINT*/ R"(
#version 410 core
in vec4 position;
void main() {
    gl_Position = position; // (x, y, z, w) <-> (x/w, y/w, z/w)
}
)";

// Fragment Shader
// It executes once per fragment (i.e. for every pixel that will be rasterized), and in part
// determines the final color that will be sent to the screen.
std::string const fragmentShaderSource = /*NOLINT*/ R"(
#version 410 core
out vec4 color;
void main() {
    color = vec4(1.1, 1.0, 0.0, 1.0);
}
)";

// If true we quit the main loop
bool quit = false; // NOLINT

} // namespace App

namespace {

void GetOpenGLVersionInfo()
{
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

/// Compiles any valid vertex, fragment, geometry, tessellation or compute shader.
///
/// @param type Determine which shader to compile
/// @param source The shader source code
/// @return id of the shader object (0 on failure)
GLuint CompileShader(GLenum type, std::string const &source)
{
    // Create shader object
    GLuint shaderObject = 0;
    shaderObject = glCreateShader(type);

    // Specify the shader source code for the object
    char const *src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);

    // Compile the shader object
    glCompileShader(shaderObject);

    // Check for compilation errors
    GLint success = GL_FALSE;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        std::array<char, MAX_GL_INFO_LOG_LEN> infoLog = {0};
        glGetShaderInfoLog(shaderObject, MAX_GL_INFO_LOG_LEN, nullptr, infoLog.data());
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog.data() << std::endl;

        // Delete broken shader object
        glDeleteShader(shaderObject);

        return 0;
    }

    return shaderObject;
}

/// Creates a graphics program object (i.e. graphics pipeline) with a vertex shader and a fragment
/// shader
///
/// @param vertexShaderSource Vertex shader source code
/// @param fragmentShaderSource Fragment shader source code
/// @return id of the program object
GLuint CreateShaderProgram(std::string const &vertexShaderSource,
                           std::string const &fragmentShaderSource)
{
    // Create a new program object
    GLuint programObject = glCreateProgram();

    // Compile shaders
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER /*enum*/, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    //- Link shader programs (.cpp + .cpp -> executable)

    // Associate (attach) the shaders to the program object
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link a program object
    glLinkProgram(programObject);

#ifdef DEBUG
    // Check the status of the link
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        std::array<char, MAX_GL_INFO_LOG_LEN> infoLog = {0};
        glGetProgramInfoLog(programObject, static_cast<GLsizei>(infoLog.size()), nullptr,
                            infoLog.data());
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog.data() << std::endl;
    }

#endif

    //- Validation

    // OpenGL requires a VAO to be bound when you validate or use a shader program that interacts
    // with vertex attributes.
    //
    // [extra] Bind the VAO before validating the program
    // [why?] b/c otherwise we get the following error:  No vertex array object bound
    glBindVertexArray(App::vertexArrayObject);

    // Validate the program
    glValidateProgram(programObject);

#ifdef DEBUG
    GLint validateStatus = GL_FALSE;
    glGetProgramiv(programObject, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE)
    {
        std::array<char, MAX_GL_INFO_LOG_LEN> infoLog = {0};
        glGetProgramInfoLog(programObject, static_cast<GLsizei>(infoLog.size()), nullptr,
                            infoLog.data());
        std::cerr << "ERROR::PROGRAM::VALIDATION_FAILED\n" << infoLog.data() << std::endl;
    }
#endif

    // [extra] Unbind the VAO after validation
    // [why?] b/c otherwise we get the following error:  No vertex array object bound
    glBindVertexArray(0);

    // Once our final program object has been created, we can detach and delete the individual
    // shaders
    glDetachShader(programObject, vertexShader);
    glDetachShader(programObject, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programObject;
}

/* Main Loop */

/// Handle user inputs (via SDL)
///
/// @return void
void Input()
{
    // Event handler object that handles events that are related to input/output events
    SDL_Event e;

    // Handle events on queue
    while (SDL_PollEvent(&e) != 0)
    {
        // If user posts an event to quit (red x button on the corner of the window)
        if (e.type == SDL_QUIT)
        {
            std::cout << "Goodbye!" << std::endl;
            App::quit = true;
        }
    }
}

/// Setting some sort of OpenGL state prior to darwing
/// Note: some of the calls may take place at different stages (post-processing) of the pipeline
///
/// @return void
void PreDraw()
{
    // Disable depth test and face culling
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Specify the view port
    glViewport(0, 0, App::screenWidth, App::screenHeight);

    // Set the clear color (background color of the screen)
    glClearColor(App::bg.r, App::bg.g, App::bg.b, App::bg.a);

    // Clear color buffer and depth buffer with the specified color above
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // NOLINT

    // Use the compiled (and linked) program that have two shaders in it
    // This sets the current shader program to be used by all the subsequent rendering commands.
    glUseProgram(App::graphicsPipelineShaderProgram);
}

/// The render function that gets called once per loop
/// Typically this includes `glDraw` related calls, and the relevant setup of buffers for those
/// calls.
///
/// @return void
void Draw()
{
    // Enable attributes (position in this case)
    glBindVertexArray(App::vertexArrayObject);

    // Select the desired VBO
    glBindBuffer(GL_ARRAY_BUFFER, App::vertexBufferObject);

    /*
        Render data (Start at the vertex at index 0 in the currently bound vertex array. Use the
                     next 3 vertices to draw a single triangle.)

        - `GL_TRIANGLES` specifies the mode in which vertices will be interpreted. It means that
           every set of three vertices will be treated as an independent triangle

        - start: This is the starting index in the array of vertices. It tells OpenGL to begin
                 drawing from the first vertex in the array (i.e., the vertex at index 0).

        - count: This is the number of vertices to be drawn. Since a single triangle consists of 3
                 vertices, specifying 3 here will draw one triangle

        This is a rendering function. It uses the current state to generate a stream of vertices
        that will form triangles.
    */
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Stop using our current graphics pipeline
    // Note: this is not necessary if we only have on graphics pipe line.
    glUseProgram(0);
}

} // namespace

/// Initialize the graphics application. It sets up a window and OpenGL context (with appropriate
/// version)
///
/// @return void
void App::Initialize()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL2 could not initialize video subsystem: " << SDL_GetError() << std::endl;
        exit(1); // NOLINT
    }

    //- Setup OpenGL context

    // Use OpenGL 4.1 core or greater (latest version available on Mac)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    // Disable deprecated functions
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering for smoother update
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create an application window using OpenGL that supports SDL
    App::graphicsApplicationWindow = SDL_CreateWindow(
        "OpenGL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, // NOLINT
        App::screenWidth, App::screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (App::graphicsApplicationWindow == nullptr)
    {
        std::cerr << "SDL window could not be created: " << SDL_GetError() << std::endl;
        exit(2); // NOLINT
    }

    // Create an OpenGL graphics context (a big struct)
    App::openGLContext = SDL_GL_CreateContext(App::graphicsApplicationWindow);

    if (App::openGLContext == nullptr)
    {
        std::cerr << "OpenGL Context could not be created: " << SDL_GetError() << std::endl;
        exit(3); // NOLINT
    }

    // Initialize GLAD library
    if (gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress)) == 0)
    {
        std::cerr << "Could not initialize Glad." << std::endl;
        exit(4); // NOLINT
    }

    // Once Glad is setup we can access OpenGL API
    GetOpenGLVersionInfo();
}

/// Setup geometry/model/mesh during vertex specification step
///
/// The first stage in the rasterization pipeline is transforming vertices to clip space. Before
/// OpenGL can do this however, it must receive a list of vertices. So the very first stage of the
/// pipeline is sending triangle data to OpenGL.
///
/// @return void
void App::VertexSpecification()
{
    // Model/Geometry/Mesh data
    // Specify the x,y,z (and w) position attributes withing vertexPositions for the data. This
    // information is stored in the CPU, and we need to store the data on the GPU in a call to
    // `glBufferData`, which will store this information into a vertex buffer object (VBO) At a
    // minimum a vertex should have a position attribute
    //
    // Since every 4 floats represents a vertex's position, we have 3 vertices: the minimum number
    // for a triangle
    std::vector<GLfloat> const vertexPosition = {
        // x      y      z      w
        -0.8F, -0.8F, +0.0F, +1.0F, // vertex 0 (left)
        +0.8F, -0.8F, +0.0F, +1.0F, // vertex 1 (right)
        +0.0F, +0.8F, +0.0F, +1.0F  // vertex 2 (top)
    };

    //- Set things up on the GPU
    // These command set up the coordinates of the triangle to be rendered. They tell OpenGL the
    // location in memory that the positions of the triangle will come from.

    // Vertex Array Object (VAO) setup
    // It can be thought of as a wrapper around all of the vertex buffer objects in the sense that
    // it encapsulates all VBO states that we are setting up. Thus, it is also important that we
    // bind (select) VAO (via `glBindVertexArray`) before VBO operations Generate 1 vertex array
    // object.
    glGenVertexArrays(1, &App::vertexArrayObject);
    // Bind to the desired VAO -> GL_ARRAY_BUFFER
    glBindVertexArray(App::vertexArrayObject);

    // Vertex Buffer Object (VBO) setup
    // Generate 1 new VBO and bind to it
    glGenBuffers(1, &App::vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, App::vertexBufferObject);

    /*
        Populate within the currently bound buffer the data from `vertexPositions` which is on the
        CPU onto a buffer that will live on the GPU.

        - target: type of the buffer (GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
        - size: Size of the data in bytes.
        - data: Raw array of data
        - usage: how we intend to use the data
    */
    glBufferData(GL_ARRAY_BUFFER, vertexPosition.size() * sizeof(GLfloat), // NOLINT
                 vertexPosition.data(), GL_STATIC_DRAW);

    //- Tell OpenGL how the information in VBO must be used.

    // ٍEnable our only (and first -> [0]) vertex attribute, which is position (x, y, z)
    // It enables the first vertex attribute of the currently bound array buffer
    glEnableVertexAttribArray(0);

    /*
        Tell OpenGL how we are going to move through the data.

        - index: Attribute 0 correspond to the enabled glEnableVertexAttribArray.
                 This also correspond to (layout=0) in shader which selects these attributes.
        - size: Number of components in this attribute (3: x, y, z)
        - type: Type of components
        - normalize: whether these numbers are normalized (between 0 and 1)
        - stride: byte offset between consecutive generic vertex attributes (in this case 3*GLfloat)
        - offset: offset of the first component of the first generic vertex attributes in the array
                 in the data store of the buffer currently bound to the GL_ARRAY_BUFFER target. The
                 initial value is 0.
    */
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
                          static_cast<void *>(nullptr));

    // Unbind currently bound VAO
    glBindVertexArray(0);

    // Disable any attribute we opened in our VAO as we do not want to leave them open.
    glDisableVertexAttribArray(0);
}

/// Once the geometry is ready, create the graphics pipeline (setting up vertex and fragment
/// shaders)
///
/// @return void
void App::CreateGraphicsPipeline()
{
    App::graphicsPipelineShaderProgram = CreateShaderProgram(App::vertexShaderSource,
                                                             App::fragmentShaderSource);
}

/// Main application (infinite) loop
///
/// @return void
void App::MainLoop()
{
    while (!quit)
    {
        // Handle inputs
        Input();

        // Setup anything prior to rendering (e.g. setting up OpenGL state)
        PreDraw();

        // Draw (rendering) calls in OpenGL
        Draw();

        // Update the screen on the specified window.
        // The OpenGL framebuffer is double-buffered: SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        // The image that are currently being shown to the user is not the same image we are
        // rendering to. Thus, all of our rendering is hidden from view until it is shown to the
        // user. This way, the user never sees a half-rendered image. This is the function that
        // causes the image we are rendering to be displayed to the user.
        SDL_GL_SwapWindow(App::graphicsApplicationWindow);
    }
}

void App::CleanUp()
{
    // Clean up SDL window
    SDL_DestroyWindow(App::graphicsApplicationWindow);

    // Clean up SDL video subsystem
    SDL_Quit();
}
