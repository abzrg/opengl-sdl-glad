// Fragment Shader

// It executes once per fragment (i.e. for every pixel that will be rasterized), and in part
// determines the final color that will be sent to the screen.

#version 410 core

// Take the vertex color as input sent by the vertex Shader
in vec3 v_vertexColor;

out vec4 color;

void main() {
    color = vec4(v_vertexColor.r, v_vertexColor.g, v_vertexColor.b, 1.0f);
}
