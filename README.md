# OpenGL + Glad + SDL2

OpenGL 4.1:
[glad.zip](https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D4.1&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=compatibility&loader=on)

## Learning Resources

- [Introduction to OpenGL -- Mike Shah](https://www.youtube.com/playlist?list=PLvv0ScY6vfd9zlZkIIqGDeG5TUWswkMox)
- [Learning Modern 3D Graphics Programming -- Jason L. McKesson (2012)](https://nicolbolas.github.io/oldtut/)
- [docs.gl](https://docs.gl/)
- [C++ OpenGL Function Call Wrapping](https://indiegamedev.net/2020/01/17/c-opengl-function-call-wrapping/)

## Winding order and front face

The position coordinates should be in the _winding order_.
Winding order is the order our vertices are laid out.
It can be clock-wise or counter-clockwise order.
The winding order tells us which direction is the front of the triangle.
It's similar to the _right hand rule_ and that's because OpenGL by default uses a _right handed coordinate system_.

[`glFrontFace`](https://docs.gl/gl4/glFrontFace) allows one to specify which direction correspond to the front face.

## Drawing a Quadrilateral

A quadrilateral is consisting of two triangles.
In vertex specification we need two sets of three coordinate points.
Two vertices have the same coordinate but have to be specified separately.
We also need to specify that we are drawing 6 vertices in the call to `glDrawArrays`.

## Error handling

```cpp
// https://community.arm.com/arm-community-blogs/b/graphics-gaming-and-vr-blog/posts/easier-opengl-es-debugging-on-arm-mali-gpus-with-gl_5f00_khr_5f00_debug

// #ifdef _DEBUG
// #define CHECK_GL_ERROR() checkGLErrorAndPrint()
// #else
// #define CHECK_GL_ERROR()
// #endif
//
// void checkGLErrorAndPrint(void)
// {
//     GLenum error = glGetError();
//     if (error != GL_NO_ERROR)
//     {
//         std::cout << ("GL error detected: 0x%04x\n", error);
//     }
// }
```
