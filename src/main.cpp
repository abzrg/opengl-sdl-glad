#include <iostream>

#include <SDL2/SDL.h>

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

    SDL_Window* window = SDL_CreateWindow(kWinTitle, kWinX, kWinY, kWinWidth, kWinHeight, SDL_WINDOW_SHOWN);

#ifdef __APPLE__
    bool quitApp = false;
    while (!quitApp) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quitApp = true;
            }
        }
    }
#else
    // Does not work on macos
    SDL_Delay(3000);
#endif

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
