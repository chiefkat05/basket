#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "shader.h"
#include <glm/glm.hpp>

extern const float width;
extern const float height;
extern const float nearView;
extern const float farView;

extern float cube[288];
extern float quad[24];

extern glm::vec3 camRotation;

extern glm::vec3 camFront;
extern glm::vec3 prevCamFront;
extern glm::vec3 camFrontAlign;
extern const glm::vec3 camUp;
extern bool firstMouseInteraction;

extern float prevMx, prevMy, mouseSen;

extern GLFWwindow *mainWindow;

namespace gfx
{
    void framebuffer_size_callback(GLFWwindow *_win, int _w, int _h);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);

    int windowInit();

    void processInput();
};

#endif