// Working with vectors
// - Printing vectors:  glm::to_string(A)
// - dot product:       glm::dot(A,B)
// - normalization:     glm::normalize(A)
// - length/magnitude:  glm::length(A)
// - cross product:     glm::cross(A,B)
// - (element-wise) addition/subtraction/division/multiplication

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

#include <cmath>
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

    std::cout << "\n";

    // Addition and subtraction:
    glm::vec3 Add = A + B;
    glm::vec3 Mult = A * B;
    glm::vec3 Divide = A / B;
    std::cout << "A + B: " << glm::to_string(Add) << std::endl;
    std::cout << "A * B: " << glm::to_string(Mult) << std::endl;
    std::cout << "A / B: " << glm::to_string(Divide) << std::endl;

    std::cout << "\n";

    // Access to components
    std::cout << "B.yz: " << glm::to_string(B.yz()) << std::endl;

    std::cout << "\n";

    // Dot product
    float d = glm::dot(A, B);
    std::cout << "d = glm::dot(A, B) = " << d << std::endl;

    std::cout << "\n";

    // Normalizing vectors
    glm::vec3 B_normalized = glm::normalize(B);
    std::cout << "B normalized: " << glm::to_string(B_normalized) << std::endl;

    std::cout << "\n";

    // Angle between two vectors
    float dotprod = glm::dot(glm::normalize(A), glm::normalize(B));
    float angle = std::acosf(dotprod) * 180 / M_PI;
    std::cout << "Angle between A and B: " << angle << std::endl;

    std::cout << "\n";

    // Length of a vector
    std::cout << "size of the vector B (B.length()): " << B.length() << std::endl;
    std::cout << "Magnitude of the vector B (glm::length(B)): " << glm::length(B) << std::endl;

    std::cout << "\n";

    // Cross product
    glm::vec3 C = glm::cross(A, B);
    glm::vec3 CNeg = glm::cross(B, A);
    std::cout << "C = cross(A,B): " << glm::to_string(C) << std::endl;
    std::cout << "C = cross(B,A): " << glm::to_string(CNeg) << std::endl;

    return 0;
}
