#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "shader.h"
#include <glm/glm.hpp>
#include <assimp/aabb.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
        glm::vec3 position, normal;
        glm::vec2 textureCoordinates;
    };
    struct texture
    {
        unsigned int id;
        std::string type;
        std::string path;
    };
    class mesh
    {
    public:
        std::vector<vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<texture> textures;

        mesh(std::vector<vertex> _vertices, std::vector<unsigned int> _indices, std::vector<texture> _textures)
        {
            this->vertices = _vertices;
            this->indices = _indices;
            this->textures = _textures;

            meshInit();
        }
        void draw(Shader &shader)
        {
            for (unsigned int i = 0; i < textures.size(); ++i)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                std::string name = textures[i].type;

                shader.setInt(("material." + name).c_str(), i);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glActiveTexture(GL_TEXTURE0);
        }

    private:
        unsigned int VAO, VBO, EBO;

        void meshInit()
        {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                         &indices[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, normal));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, textureCoordinates));

            glBindVertexArray(0);
        }
    };
    class model
    {
    public:
        model(std::string path)
        {
            loadModel(path);
        }
        void draw(Shader &shader)
        {
            for (unsigned int i = 0; i < meshes.size(); ++i)
            {
                meshes[i].draw(shader);
            }
        }

        std::string fullPath;

        std::vector<mesh> meshes;

    private:
        std::string directory;
        std::vector<texture> loaded_textures;

        void loadModel(std::string path);

        void processNode(aiNode *node, const aiScene *scene);
        mesh processMesh(aiMesh *_mesh, const aiScene *_scene);
        std::vector<texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                  std::string typeName);
    };

    void
    framebuffer_size_callback(GLFWwindow *_win, int _w, int _h);
    void mouse_callback(GLFWwindow *window, double xpos, double ypos);

    int windowInit();

    void processInput();
};

#endif