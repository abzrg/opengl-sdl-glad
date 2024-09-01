#include "App/App.h"
#include "SDL2/SDL.h"
#include "glad/glad.h"
#include <iostream>

constexpr int App::screenHeight = 480;
constexpr int App::screenWidth = 640;

SDL_Window *App::graphicsApplicationWindow = nullptr; // NOLINT
SDL_GLContext App::openGLContext = nullptr;           // NOLINT

namespace {

// If true we quit the main loop
bool quit = false; // NOLINT

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

void PreDraw() {}
void Draw() {}

void GetOpenGLVersionInfo()
{
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
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
