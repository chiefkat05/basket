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
        void draw(Shader &shader);

    private:
        unsigned int VAO, VBO, EBO;

        void meshInit();
    };
    // class model
    // {
    // public:
    //     model(std::string path)
    //     {
    //         loadModel(path);
    //     }
    //     void draw(Shader &shader)
    //     {
    //         for (unsigned int i = 0; i < meshes.size(); ++i)
    //         {
    //             meshes[i].draw(shader);
    //         }
    //     }

    //     std::string fullPath;

    //     std::vector<mesh> meshes;

    // private:
    //     std::string directory;
    //     std::vector<texture> loaded_textures;

    //     void loadModel(std::string path);

    //     // void processNode(aiNode *node, const aiScene *scene);
    //     // mesh processMesh(aiMesh *_mesh, const aiScene *_scene);
    //     // std::vector<texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
    //     //                                           std::string typeName); // tinyobjloader documentation time
    // };

    void
    framebuffer_size_callback(GLFWwindow *_win, int _w, int _h);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);

    int windowInit();

    void processInput();
};

#endif