#pragma once

#include "SDL2/SDL.h"

namespace App {

extern const int screenHeight; // NOLINT
extern const int screenWidth; // NOLINT

extern SDL_Window *graphicsApplicationWindow; // NOLINT
extern SDL_GLContext openGLContext; // NOLINT

void Initialize();
void MainLoop();
void CleanUp();

} // namespace App
