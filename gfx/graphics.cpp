#include <math.h>
#include "graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "../nms/stb_image.h"

const float width = 1280.0f;
const float height = 720.0f;
const float nearView = 0.01f;
const float farView = 100.0f;
float cube[288] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

extern float quad[24] = {
    -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f};

void framebuffer_size_callback(GLFWwindow *_win, int _w, int _h);

glm::vec3 camRotation = glm::vec3(-90.0f, 0.0f, 0.0f);

glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 prevCamFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camFrontAlign = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 camUp = glm::vec3(0.0, 1.0, 0.0);
bool firstMouseInteraction = true;

float prevMx = 640, prevMy = 360, mouseSen = 0.1f;
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

GLFWwindow *mainWindow;

void gfx::framebuffer_size_callback(GLFWwindow *_win, int _w, int _h)
{
    glViewport(0, 0, _w, _h);
}

void gfx::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouseInteraction)
    {
        prevMx = xpos;
        prevMy = ypos;
        firstMouseInteraction = false;
    }

    float xoffset = xpos - prevMx;
    float yoffset = prevMy - ypos;
    prevMx = xpos;
    prevMy = ypos;

    xoffset *= mouseSen;
    yoffset *= mouseSen;

    camRotation.x += xoffset;
    camRotation.y += yoffset;

    if (camRotation.y > 89.9f)
        camRotation.y = 89.9f;
    if (camRotation.y < -89.9f)
        camRotation.y = -89.9f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(camRotation.x)) * cos(glm::radians(camRotation.y));
    direction.y = sin(glm::radians(camRotation.y));
    direction.z = sin(glm::radians(camRotation.x)) * cos(glm::radians(camRotation.y));

    glm::vec3 alignDir = glm::vec3(0.0f, 0.0f, 0.0f);
    alignDir.x = cos(glm::radians(camRotation.x));
    alignDir.z = sin(glm::radians(camRotation.x));

    camFront = glm::normalize(direction);
    camFrontAlign = glm::normalize(alignDir);
}

int gfx::windowInit()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mainWindow = glfwCreateWindow(width, height, "basket engine alpha", NULL, NULL);
    if (mainWindow == NULL)
    {
        std::cerr << "GLFW failed to create a window." << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(mainWindow);

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
    {
        std::cerr << "GLAD failed to load." << std::endl;
        return 1;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);

    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(mainWindow, mouse_callback);

    return 0;
}

void gfx::processInput()
{
    if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(mainWindow, true);
}