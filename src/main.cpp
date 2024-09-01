/* OpenGL + GLAD + SDL2 */
/* Following tutorials from Mike Shah */
/* g++ main.cpp helper.cpp -o prog -lSDL2 -ldl */

#include "App/App.h"


int main()
{
    App::Initialize();
    App::MainLoop();
    App::CleanUp();

    return 0;
}
