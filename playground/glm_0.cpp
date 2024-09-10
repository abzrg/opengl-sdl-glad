// Computing dot and cross product

// NOTE: Faster program compilation
//
// GLM is a header only library that makes a heavy usage of C++ templates. This
// design may significantly increase the compile time for files that use
// GLM. Hence, it is important to limit GLM inclusion to header and source files
// that actually use it. Likewise, GLM extensions should be included only in
// program sources using them. i.e. dont #include <glm/glm.hpp>

// GLM will try to detect the language version but it can be specified
// explicitly.  Compiler error will be emitted if the compiler does not support
// the specified language version.
#define GLM_FORCE_CXX17

#define GLM_FORCE_SWIZZLE
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <iostream>

int main()
{
    // 3D vectors
    glm::vec3 A(1.0F, 1.0F, 1.0F); // or: glm::vec3 A(1.0f);
    glm::vec3 B(1.5F, 3.5F, 4.0F);

    // 4x4 matrices
    glm::mat4 M(1.0F);

    // Printing vectors and matrices
    std::cout << "A: " << glm::to_string(A) << std::endl;
    std::cout << "B: " << glm::to_string(B) << std::endl;
    std::cout << "M: " << glm::to_string(M) << std::endl;

    // Access to components
    std::cout << "B.yz: " << glm::to_string(B.yz()) << std::endl;

    // Dot product
    float d = glm::dot(A, B);
    std::cout << "d = glm::dot(A, B) = " << d << std::endl;

    // Normalizing vectors
    glm::vec3 B_normalized = glm::normalize(B);
    std::cout << "B normalized: " << glm::to_string(B_normalized) << std::endl;

    // Length of a vector
    std::cout << "size of the vector B (which is 3 for vec3): " << A.length() << std::endl;

    // Cross product
    glm::vec3 C = glm::cross(A, B);
    std::cout << "C: " << glm::to_string(C) << std::endl;

    return 0;
}
