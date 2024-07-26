#include <math.h>
#include "graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "../nms/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../nms/tiny_obj_loader.h"

const float width = 1280.0f;
const float height = 720.0f;
const float nearView = 0.01f;
const float farView = 100.0f;

unsigned int gfx::loadTexture(const char *path, const std::string &directory)
{
    std::string realPath = std::string(path);
    realPath = directory + '/' + realPath;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(realPath.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrComponents == 3)
            format = GL_RGB;
        if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << realPath << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void framebuffer_size_callback(GLFWwindow *_win, int _w, int _h);

glm::vec3 camRotation = glm::vec3(-90.0f, 0.0f, 0.0f);

glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 prevCamFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camFrontAlign = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camRight = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 camTrueRight = glm::vec3(1.0f, 0.0f, 0.0f);
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
    camRight = glm::cross(camFront, camUp);
    camTrueRight = glm::cross(camFrontAlign, camUp);
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

gfx::mesh::mesh(std::string dir, std::string path)
{

    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = dir;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(std::string(dir + path), reader_config))
    {
        if (!reader.Error().empty())
        {
            std::cout << "Failed to load model: " << reader.Error();
        }
        return;
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjLoader warning: " << reader.Warning();
    }

    directory = dir;
    name = path;

    auto &attributes = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &materials = reader.GetMaterials();

    for (unsigned int s = 0; s < shapes.size(); ++s)
    {
        for (unsigned int f = 0; f < shapes[s].mesh.indices.size(); ++f)
        {
            tinyobj::index_t idx = shapes[s].mesh.indices[f];
            vertex tmpV;

            if (idx.vertex_index < 0)
                continue;

            tmpV.position.x = attributes.vertices[idx.vertex_index * 3];
            tmpV.position.y = attributes.vertices[idx.vertex_index * 3 + 1];
            tmpV.position.z = attributes.vertices[idx.vertex_index * 3 + 2];

            float vertexDist = abs(glm::length(tmpV.position));
            if (vertexDist > boundingSphereRadius)
            {
                boundingSphereRadius = vertexDist;
            }

            if (idx.normal_index < 0)
                continue;

            tmpV.normal.x = attributes.normals[idx.normal_index * 3];
            tmpV.normal.y = attributes.normals[idx.normal_index * 3 + 1];
            tmpV.normal.z = attributes.normals[idx.normal_index * 3 + 2];

            if (idx.texcoord_index < 0)
                continue;

            tmpV.textureCoordinates.x = attributes.texcoords[idx.texcoord_index * 2];
            tmpV.textureCoordinates.y = -attributes.texcoords[idx.texcoord_index * 2 + 1];

            vertices.push_back(tmpV);
        }
        std::string str = "diffuse.png";
        tex.id = loadTexture(str.c_str(), dir);
        tex.type = "diffuse";
        tex.path = str.c_str();
    }

    meshInit();
}

void gfx::mesh::meshInit()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)offsetof(vertex, textureCoordinates));

    glBindVertexArray(0);
}
void gfx::mesh::draw(Shader &shader, bool customTexture)
{
    shader.use();
    if (customTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        std::string name = tex.type;

        shader.setInt(("material." + name).c_str(), 0);
        glBindTexture(GL_TEXTURE_2D, tex.id);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

// void gfx::model::processNode(aiNode *node, const aiScene *scene)
// {
//     for (unsigned int i = 0; i < node->mNumMeshes; ++i)
//     {
//         aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
//         meshes.push_back(processMesh(mesh, scene));
//     }
//     for (unsigned int i = 0; i < node->mNumChildren; ++i)
//     {
//         processNode(node->mChildren[i], scene);
//     }
// }
// gfx::mesh gfx::model::processMesh(aiMesh *_mesh, const aiScene *_scene)
// {
//     std::vector<vertex> vertices;
//     std::vector<unsigned int> indices;
//     std::vector<texture> textures;
//     std::vector<aiFace> faces;

//     for (unsigned int i = 0; i < _mesh->mNumVertices; ++i)
//     {
//         vertex _vertex;

//         _vertex.position = glm::vec3(_mesh->mVertices[i].x,
//                                      _mesh->mVertices[i].y,
//                                      _mesh->mVertices[i].z);
//         _vertex.normal = glm::vec3(_mesh->mNormals[i].x,
//                                    _mesh->mNormals[i].y,
//                                    _mesh->mNormals[i].z);

//         if (_mesh->mTextureCoords[0])
//         {
//             _vertex.textureCoordinates = glm::vec2(
//                 _mesh->mTextureCoords[0][i].x,
//                 _mesh->mTextureCoords[0][i].y);
//         }
//         else
//         {
//             _vertex.textureCoordinates = glm::vec2(0.0f, 0.0f);
//         }

//         vertices.push_back(_vertex);
//     }

//     for (unsigned int i = 0; i < _mesh->mNumFaces; ++i)
//     {
//         aiFace face = _mesh->mFaces[i];

//         for (unsigned int j = 0; j < face.mNumIndices; ++j)
//             indices.push_back(face.mIndices[j]);
//     }

//     if (_mesh->mMaterialIndex >= 0)
//     {
//         aiMaterial *material = _scene->mMaterials[_mesh->mMaterialIndex];

//         std::vector<texture> diffuseMaps = loadMaterialTextures(material,
//                                                                 aiTextureType_DIFFUSE, "diffuse");
//         textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//         std::vector<texture> specularMaps = loadMaterialTextures(material,
//                                                                  aiTextureType_SPECULAR, "specular");
//         textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//     }

//     return mesh(vertices, indices, textures);
// }
// std::vector<gfx::texture> gfx::model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
//                                                            std::string typeName)
// {
//     std::vector<texture> textures;
//     for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
//     {
//         aiString str;
//         mat->GetTexture(type, i, &str);
//         bool loaded = false;
//         for (unsigned int j = 0; j < loaded_textures.size(); ++j)
//         {
//             if (std::strcmp(loaded_textures[j].path.data(), str.C_Str()) == 0)
//             {
//                 textures.push_back(loaded_textures[j]);
//                 loaded = true;
//                 break;
//             }
//         }

//         if (!loaded)
//         {
//             texture _texture;
// _texture.id = loadTexture(str.C_Str(), directory);
//             _texture.type = typeName;
//             _texture.path = str.C_Str();
//             textures.push_back(_texture);
//             loaded_textures.push_back(_texture);
//         }
//     }
//     return textures;
// }
