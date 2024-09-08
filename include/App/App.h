#pragma once

#include "SDL2/SDL.h"
#include "glad/glad.h"

#define DEBUG
#define MAX_GL_INFO_LOG_LEN 512

namespace App {

extern int const screenHeight; // NOLINT
extern int const screenWidth;  // NOLINT

extern SDL_Window *graphicsApplicationWindow; // NOLINT
extern SDL_GLContext openGLContext;           // NOLINT

extern GLuint vertexArrayObject;  // NOLINT
extern GLuint vertexBufferObjectPosition; // NOLINT
extern GLuint vertexBufferObjectColor; // NOLINT

extern GLuint graphicsPipelineShaderProgram; // NOLINT

void Initialize();
void VertexSpecification();
void CreateGraphicsPipeline();
void MainLoop();
void CleanUp();

} // namespace App
