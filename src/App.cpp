#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
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
GLuint vertexBufferObject = 0; // Containing position and color -- NOLINT

// Index/Element Buffer Object (IBO i.e. EBO)
GLuint indexBufferObject = 0; // NOLINT

// Shader program object
// This object stores a unique id for the graphic pipeline program object that will be used for our
// OpenGL draw calls
GLuint graphicsPipelineShaderProgram = 0; // NOLINT

/* At a minimum, every Modern OpenGL program needs a vertex and fragment shader
   OpenGL provides functions that will compile the shader source code (stored as strings) at
   run-time. */

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

/// Clear the error state until no error exists.
/// This is because in OpenGL and a call to glGetError no other error is recorded until.
/// 1. glGetError is called,
/// 2. the error code is returned,
/// 3. the flag is set to GL_NO_ERROR.
///
/// @return void
void GLClearAllErrors()
{
    // GL_NO_ERROR: no error has been recorded
    while (glGetError() != GL_NO_ERROR)
    {

    }
}

/// Check if an error has occured and return the error code and show where it happend.
/// This function is usually called after a call to some OpenGL function (starting with gl).
///
/// @param funcName name of the function in which the error has happened
/// @param lineNo line number at which the error has happend
/// @return bool whether any error has occurred or not
bool GLCheckErrorStatus(const char* funcName, int lineNo)
{
    while (GLenum error = glGetError()) {
        std::cerr << "OpenGL Error: " << error
                  << "\n\tLine: " << lineNo
                  << "\n\tFunction: " << funcName
                  << '\n' << std::endl;
        return true;
    }

    // error was 0 (GL_NO_ERROR)
    return false;
}

/// Wraps call to OpenGL functions. First clears any previously set error state,
/// then it executes the function call, and finally checks for the errors,
/// passing the string of the function call (#X) and the line number (__LINE__)
/// at which the error has happened.
///
/// @param X function call including function name all its parameters
#define GLCall(X)                               \
    GLClearAllErrors();                         \
    X;                                          \
    GLCheckErrorStatus(#X, __LINE__);

std::string LoadShaderAsString(std::string const &filepath)
{
    // Holds the returning shader program string
    std::string src{};

    std::string line;
    std::ifstream file(filepath);
    if (file.is_open())
    {
        while (std::getline(file, line))
        {
            src += line + '\n';
        }

        file.close();
    }

    return src;
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

    std::array<GLuint, 2> shaderList = {
        vertexShader,
        fragmentShader,
    };

    //- Link shader programs (.cpp + .cpp -> executable)

    // Associate (attach) the shaders to the program object
    std::for_each(shaderList.begin(), shaderList.end(),
                  [&](GLuint shader) { glAttachShader(programObject, shader); });

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
    std::for_each(shaderList.begin(), shaderList.end(),
                  [&](GLuint shader) { glDetachShader(programObject, shader); });
    std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

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

    // Draw vertices specified in the index buffer
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);); // Checking OpenGL errors

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
    // Specify the x,y,z position and r,g,b color attributes within vertexPositions for the data.
    // This information is stored in the CPU, and we need to store the data on the GPU in a call to
    // `glBufferData`, which will store this information into a vertex buffer object (VBO).
    // At a minimum a vertex should have a position attribute.

    // Vertex data/spec
    // - position is a value between -1.0 and 1.0 (clip space)
    // - color is a value between 0.0 and 1.0
    std::vector<GLfloat> const vertexData = {
        -0.5F, -0.5F, +0.0F, // vertex 0 - position (bottom left) <x, y, z>
        +1.0F, +0.0F, +0.0F, // vertex 0 - color                  <r, g, b>

        +0.5F, -0.5F, +0.0F, // vertex 1 - position (bottom right)
        +0.0F, +1.0F, +0.0F, // vertex 1 - color

        -0.5F, +0.5F, +0.0F, // vertex 2 - position (top left)
        +1.0F, +1.0F, +0.0F, // vertex 2 - color

        +0.5F, +0.5F, +0.0F, // vertex 3 - position (top right)
        +0.0F, +0.0F, +1.0F, // vertex 3 - color
    };

    //- Set things up on the GPU

    // The following command set up the coordinates of the triangle to be rendered.
    // They tell OpenGL the location in memory (more specifically in VAO) that a certain atribute of
    // a vertex will come from.

    // Vertex Array Object (VAO) setup:
    // It can be thought of as a wrapper around all of the vertex buffer objects in the sense that
    // it encapsulates all VBO states that we are setting up.
    // Thus, it is also important that we bind (select) VAO (via `glBindVertexArray`) before VBO
    // operations Generate 1 vertex array object.
    glGenVertexArrays(1, &App::vertexArrayObject);
    // Bind to the desired VAO -> GL_ARRAY_BUFFER
    glBindVertexArray(App::vertexArrayObject);

    // Vertex Buffer Object (VBO) setup
    // Generate 1 new VBO and bind to it to store vertex positions and colors
    glGenBuffers(1, &App::vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, App::vertexBufferObject);

    // Populate within the currently bound buffer the data from `vertexPositions` which is on the
    // CPU onto a buffer that will live on the GPU.
    // 1. It allocates memory for the buffer currently bound to GL_ARRAY_BUFFER
    // 2. Copying data from our memory array into the buffer object
    // After this function call, the buffer object stores exactly what vertexPositions stores.
    //
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), // NOLINT
                 vertexData.data(), GL_STATIC_DRAW);

    // Index/Element Buffer Object (IBO i.e. EBO)
    std::vector<GLuint> const indexBufferData{
        2, 0, 1, // First triangle
        3, 2, 1, // Second triangle
    };
    glGenBuffers(1, &App::indexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App::indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indexBufferData.size() * sizeof(GLuint)),
                 indexBufferData.data(), GL_STATIC_DRAW);

    //- So far we managed to put the vertex data in GPU's memory. However VBO is not formatted.
    // Now we need to tell OpenGL what form the vertex data in VBO takes.

    // Specify position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);

    // Specify Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat))); // NOLINT

    //- Now that OpenGL knows where to find the data and how to interpret it

    //- Clean Up

    // Unbind currently bound VAO
    glBindVertexArray(0);

    // Disable any attribute we opened in our VAO as we do not want to leave them open.
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

/// Once the geometry is ready, create the graphics pipeline (setting up vertex and fragment
/// shaders)
///
/// @return void
void App::CreateGraphicsPipeline()
{
    std::string vertexShaderSource = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("./shaders/frag.glsl");

    App::graphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource,
                                                             fragmentShaderSource);
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
