#include <iostream>
#include <stdexcept>
#include <vector>

#include "SDL2/SDL.h"
#include "glad/glad.h"

#include "App/App.h"

constexpr int App::screenHeight = 480;
constexpr int App::screenWidth = 640;

SDL_Window *App::graphicsApplicationWindow = nullptr; // NOLINT
SDL_GLContext App::openGLContext = nullptr;           // NOLINT

// VAO
GLuint App::vertexArrayObject = 0; // NOLINT
// VBO
GLuint App::vertexBufferObject = 0; // NOLINT

// Program object (for shaders)
GLuint App::graphicsPipelineShaderProgram = 0; // NOLINT

namespace {

std::string const vertexShaderSource = R"(
#version 410 core
in vec4 position;
void main() {
    gl_Position = position;
}
)";

std::string const fragmentShaderSource = R"(
#version 410 core
out vec4 color;
void main() {
    color = vec4(1.1, 1.0, 0.0, 1.0);
}
)";

// If true we quit the main loop
bool quit = false; // NOLINT

void GetOpenGLVersionInfo()
{
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

// Compiles the shader and returns a handle
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
    GLint success = 0;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
    if (success == 0)
    {
        std::array<char, 512> infoLog = {0};
        glGetShaderInfoLog(shaderObject, 512, nullptr, infoLog.data());
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog.data() << std::endl;
    }

    return shaderObject;
}

// Returns the handle to the GPU program
GLuint CreateShaderProgram(std::string const &vertexShaderSource,
                           std::string const &fragmentShaderSource)
{
    // Create an empty program and return a handle
    GLuint programObject = glCreateProgram();

    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER /*enum*/, vertexShaderSource);
    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Associate (attach) the shaders to the program object
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link a program object
    glLinkProgram(programObject);

#ifdef DEBUG
    // Check the status of the link
    GLint linkStatus = 0;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == 0)
    {
        std::array<char, 512> infoLog = {0};
        glGetProgramInfoLog(programObject, static_cast<GLsizei>(infoLog.size()), nullptr,
                            infoLog.data());
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog.data() << std::endl;
    }

#endif

    // OpenGL requires a VAO to be bound when you validate or use a shader program that interacts
    // with vertex attributes.
    //
    // [extra] Bind the VAO before validating the program
    // [why?] b/c otherwise we get the following error:  No vertex array object bound
    glBindVertexArray(App::vertexArrayObject);

    // Validate the program
    glValidateProgram(programObject);

#ifdef DEBUG
    GLint validateStatus = 0;
    glGetProgramiv(programObject, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == 0)
    {
        std::array<char, 512> infoLog = {0};
        glGetProgramInfoLog(programObject, static_cast<GLsizei>(infoLog.size()), nullptr,
                            infoLog.data());
        std::cerr << "ERROR::PROGRAM::VALIDATION_FAILED\n" << infoLog.data() << std::endl;
    }

#endif

    // [extra] Unbind the VAO after validation
    // [why?] b/c otherwise we get the following error:  No vertex array object bound
    glBindVertexArray(0);

    // Clean up shader objects after linking
    glDetachShader(programObject, vertexShader);
    glDetachShader(programObject, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programObject;
}

/* Main Loop */

// Handle inputs (via SDL)
void Input()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            std::cout << "Goodbye!" << std::endl;
            quit = true;
        }
    }
}

// Setting OpenGL state for darwing
void PreDraw()
{
    // ?
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Set the view port ?
    glViewport(0, 0, App::screenWidth, App::screenHeight);

    // Color of the background
    glClearColor(0.0F, 0.0F, 1.F, 1.0F);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT); // NOLINT

    // ?
    glUseProgram(App::graphicsPipelineShaderProgram);
}

// Choosing which vertex array object we are going to draw
void Draw()
{
    glBindVertexArray(App::vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, App::vertexBufferObject);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

} // namespace

void App::Initialize()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL2 could not initialize video subsystem." << std::endl;
        exit(1); // NOLINT
    }

    // Use OpenGL 4.1 (latest version available on Mac)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // Disable deprecated functions
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering for smoother transitioning
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // ?
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create window
    graphicsApplicationWindow = SDL_CreateWindow("OpenGL Window", 0, 0, screenWidth, screenHeight,
                                                 SDL_WINDOW_OPENGL);
    if (graphicsApplicationWindow == nullptr)
    {
        std::cerr << "SDL window was not able to be created." << std::endl;
        exit(2); // NOLINT
    }

    // Create OpenGL context
    openGLContext = SDL_GL_CreateContext(graphicsApplicationWindow);
    if (openGLContext == nullptr)
    {
        std::cerr << "OpenGL Context not available." << std::endl;
        exit(3); // NOLINT
    }

    // Initialize Glad library
    if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0)
    {
        std::cerr << "Could not initialize Glad." << std::endl;
        exit(4); // NOLINT
    }

    // Once Glad is setup we can access OpenGL API
    GetOpenGLVersionInfo();
}

// Creates vertices on the GPU
void App::VertexSpecification()
{
    // Specify the x,y,z component of the vertex position (lives on CPU)
    std::vector<GLfloat> const vertexPosition = {
        // x     y     z
        -0.8F, -0.8F, +0.0F, // vertex 0
        +0.8F, -0.8F, +0.0F, // vertex 1
        +0.0F, +0.8F, +0.0F  // vertex 2
    };

    //- Set things up on the GPU

    // Generate 1 vertex array object (VAO)
    glGenVertexArrays(1, &App::vertexArrayObject);
    // Bind to the desired VAO -> GL_ARRAY_BUFFER
    glBindVertexArray(App::vertexArrayObject);

    // Generate 1 vertex buffer object (VBO)
    glGenBuffers(1, &App::vertexBufferObject);
    // Bind to the desired VBO for the target of vertex attributes (position, color, ...)
    glBindBuffer(GL_ARRAY_BUFFER, App::vertexBufferObject);
    // Populate the VBO with our data
    glBufferData(GL_ARRAY_BUFFER,                         // Target
                 vertexPosition.size() * sizeof(GLfloat), // NOLINT (size in bytes of all data)
                 vertexPosition.data(),                   // Returns ptr to the underlying array
                 GL_STATIC_DRAW // Usage (for static drawing: not much change)
    );

    // ٍEnable our only vertex attribute (position (x, y, z)), which is the first one -> 0
    // Enable the vertex attribute of the currently bound array buffer
    glEnableVertexAttribArray(0);
    /*
       Actually use the data that is bound the target of GL_ARRAY_BUFFER

       - index:  index-th attribute
       - size:   how many items are in this vertex attribute (3 numbers: x, y, z)
       - type:   type of the data in this vertex attribute
       - normalize: whether these numbers are normalized (between 0 and 1) or not
       - stride: byte offset between consecutive generic vertex attributes (in this case 3*GLfloat)
       - offset: offset of the first component of the first generic vertex attributes in the array
                 in the data store of the buffer currently bound to the GL_ARRAY_BUFFER target. The
                 initial value is 0.
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);

    // Unbind VAO
    glBindVertexArray(0);
    // Disable vertex attribute of the currently bound array buffer
    glDisableVertexAttribArray(0);
}

// Once the geometry is ready, create pipeline (setting up vertex and fragment shaders)
void App::CreateGraphicsPipeline()
{
    App::graphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource,
                                                             fragmentShaderSource);
}

void App::MainLoop()
{
    while (!quit)
    {
        Input();
        PreDraw();
        Draw();

        // Update the screen (front buffer) when drawing is done in the back buffer
        SDL_GL_SwapWindow(App::graphicsApplicationWindow);
    }
}

void App::CleanUp()
{
    // Clean up SDL window
    SDL_DestroyWindow(graphicsApplicationWindow);

    // Clean up SDL video subsystem
    SDL_Quit();
}
