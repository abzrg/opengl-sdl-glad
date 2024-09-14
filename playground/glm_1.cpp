// Transformation

// Local and World view. We move from local to world

// Resources:
// 1. https://learnopengl.com/Getting-started/Transformations

#define GLM_FORCE_SWIZZLE

// #include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <iostream>

// Prints each column in a transformation model
void print_mat4(glm::mat4 const &model)
{
    for (size_t i = 0, len = model.length(); i < len; ++i)
    {
        std::cout << "model[" << i << "]: " << glm::to_string(model[i]) << std::endl;
    }
}

int main()
{
    // Create a vertex (i.e. point)
    // This is the local coordinate.
    // 1.0f at the end is the w coordinate
    // w = 1 means we have a position (w = 0 means we have a vector)
    glm::vec4 vertex(1.0F, 5.0F, 1.0, 1.0F);

    // Create a model matrix for our geometry
    // Initialize with '1' for identity matrix
    // NOTE: do not count on GLM to provide you an identity matrix.
    glm::mat4 model(1.0F);

    // Perform some transformations (i.e. moving us inthe world)
    // We are now in the 'world space'

    // //- Scalaing Matrix
    // // Needs to specify:
    // // 1. identity matrix or any other mat4 matrix
    // // 2. diagonl scalar vector
    // model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

    //- Rotation Matrix
    // Needs to specify:
    // 1. identity matrix or any other mat4 matrix
    // 2. angle of rotation (in radians)
    // 3. axis of rotation (normalized unit vector)
    model = glm::rotate(glm::mat4(1.0F), glm::radians(180.0F), glm::vec3(0, 1, 0));

    // //- Translate Matrix
    // // Needs to specify:
    // // 1. identity matrix or any other mat4 matrix
    // // 2. Amount of translation in each direction of the space
    // model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));

    // Print the model
    print_mat4(model);

    // Now we apply our 'model' matrix to the vertex
    glm::vec4 world_space_vertex = model * vertex;
    std::cout << '\n'
              << "Our vertex in world space\n"
              << glm::to_string(world_space_vertex) << std::endl;

    return 0;
}
