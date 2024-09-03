/* OpenGL + GLAD + SDL2 */
/* Following tutorials from Mike Shah */
/* g++ main.cpp helper.cpp -o prog -lSDL2 -ldl */

#include "App/App.h"

int main()
{
    // 1. Setup windowing system and graphics program
    App::Initialize();

    // 2. Setup the geometry
    App::VertexSpecification();

    // 3. Create the graphics pipline (vertex and fragment shader)
    App::CreateGraphicsPipeline();

    // 4. Call the main application loop (handling input and drawing commands)
    App::MainLoop();

    // 5. Call to clean up functions when objects are not needed
    App::CleanUp();

    return 0;
}
