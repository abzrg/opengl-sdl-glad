#include <iostream>

#include <SDL2/SDL.h>
#include <glad/glad.h>

namespace {

constexpr auto kWinX{ 0 };
constexpr auto kWinY{ 0 };
constexpr auto kWinHeight{ 480 };
constexpr auto kWinWidth{ 640 };
constexpr auto kWinTitle{ "OpenGL Window" };

} // namespace

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not be initialized: " << SDL_GetError();
    } else {
        std::cout << "SDL video system is ready to go\n";
    }

    // Set OpenGL version, before creating window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window = SDL_CreateWindow(kWinTitle, kWinX, kWinY, kWinWidth, kWinHeight, SDL_WINDOW_SHOWN);

    // Setup the OpenGL graphics context
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // Load OpenGL functions
    gladLoadGLLoader(SDL_GL_GetProcAddress);

    bool quitApp = false;
    while (!quitApp) {
        glViewport(kWinX, kWinY, kWinWidth, kWinHeight);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quitApp = true;
            }

            if (event.type == SDL_MOUSEMOTION) {
                std::clog << "LOG: MOUSE HAS BEEN MOVED.\n";
            }

            if (event.type == SDL_KEYDOWN) {
                std::clog << "LOG: KEY HAS BEEN PRESSED.\n";
                if (event.key.keysym.sym == SDLK_0) {
                    std::clog << "LOG: KEY_0 WAS PRESSED.\n";
                } else {
                    std::clog << "LOG: KEY_0 WAS NOT PRESSED.\n";
                }
            }

            // Retrieve the state of all of the keys to query the scan code of
            // one or more keys at a time
            const Uint8* state = SDL_GetKeyboardState(nullptr);
            if (state[SDL_SCANCODE_RIGHT]) {
                std::clog << "RIGHT ARROW KEY IS PRESSED.\n";
            }
        }

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
