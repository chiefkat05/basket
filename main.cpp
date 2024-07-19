/**
 *
 *
 * A few tutorials were used in the creation of this engine. Most notably:
 *
 * https://www.learnopengl.com/
 *
 * https://winter.dev/articles/gjk-algorithm/
 * https://winter.dev/articles/epa-algorithm/
 *
 * https://www.geeksforgeeks.org/octree-insertion-and-searching/
 *
 *
 **/

#include "logic/game.h"

int main()
{
    gameInit();

    mainLoop();

    terminate();

    return 0;
}
