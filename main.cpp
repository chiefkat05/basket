#include "version.h"
#include "./gfx/graphics.h"

int main()
{
    std::cout << "version is " << basket_VERSION_MAJOR
              << "." << basket_VERSION_MINOR << "\n";

    if (gfx::windowInit() != 0)
    {
        std::cerr << "There was an error, and the program could not be run." << std::endl;
        return 1;
    }

    gfx::mainLoop();

    glfwTerminate();

    return 0;
}
