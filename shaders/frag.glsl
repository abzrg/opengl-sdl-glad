// Fragment Shader

// It executes once per fragment (i.e. for every pixel that will be rasterized), and in part
// determines the final color that will be sent to the screen.

#version 410 core

out vec4 color;

void main() {
    color = vec4(1.1, 1.0, 0.0, 1.0);
}
