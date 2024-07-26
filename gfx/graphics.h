#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "shader.h"
#include <glm/glm.hpp>

extern const float width;
extern const float height;
extern const float nearView;
extern const float farView;

extern float cube[288];

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
    unsigned int loadTexture(const char *path, const std::string &directory);

    struct vertex
    {
        glm::vec3 position = glm::vec3(0.0f), normal = glm::vec3(0.0f);
        glm::vec2 textureCoordinates = glm::vec2(0.0f);
    };
    struct texture
    {
        unsigned int id = 0;
        std::string type = "";
        std::string path = "";
    };
    class mesh
    {
    public:
        std::vector<vertex> vertices;
        // std::vector<unsigned int> indices;
        texture tex;

        float boundingSphereRadius = 0.0f;

        std::string directory = "", name = "";

        // mesh(std::vector<vertex> _vertices, std::vector<unsigned int> _indices, std::vector<texture> _textures)
        // {
        //     this->vertices = _vertices;
        //     this->indices = _indices;
        //     this->textures = _textures;

        //     meshInit();
        // }
        mesh(std::string dir, std::string path);
        void draw(Shader &shader, bool customTexture = true);

    private:
        unsigned int VAO, VBO, EBO;

        void meshInit();
    };

    struct fbuffer
    {
        unsigned int fbo, texture;
        fbuffer()
        {
            glGenFramebuffers(1, &fbo);

            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

            unsigned int rbo;
            glGenRenderbuffers(1, &rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 720);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cout << "framebuffer failed\n";
                return;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void fdelete()
        {

            glDeleteFramebuffers(1, &fbo);
        }
    };

    void framebuffer_size_callback(GLFWwindow *_win, int _w, int _h);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);

    int windowInit();

    void processInput();
};

#endif