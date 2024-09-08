// Vertex Shader

// It executes once per vertex, and will be in charge of the final position of the vertex

/* Homogeneous coordinates:
    A three-dimensional euclidean space point (x, y, z) becomes the homogeneous vertex with
    coordinates (x, y, z, 1.0), and the two-dimensional euclidean point (x, y) becomes (x, y,
    0.0, 1.0). As long as w is nonzero, the homogeneous vertex (x, y, z, w) corresponds to the
    three-dimensional point (x/w, y/w, z/w). */

#version 410 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexColor;

out vec3 v_vertexColor; // (convection) v_: coming from vertex shader

void main() {
    // (x, y, z, w)
    gl_Position = vec4(vertexPosition, 1.0f);

    // Sent the color down to the pipeline
    v_vertexColor = vertexColor;
}
