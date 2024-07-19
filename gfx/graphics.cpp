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

float quad[24] = {
    -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f};

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
        // if (nrComponents == 1)
        //     format = GL_RED;
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

void gfx::model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Assimp failed to load model: " << import.GetErrorString() << "\n";
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    fullPath = path;

    processNode(scene->mRootNode, scene);
}

void gfx::model::processNode(aiNode *node, const aiScene *scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}
gfx::mesh gfx::model::processMesh(aiMesh *_mesh, const aiScene *_scene)
{
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<texture> textures;
    std::vector<aiFace> faces;

    for (unsigned int i = 0; i < _mesh->mNumVertices; ++i)
    {
        vertex _vertex;

        _vertex.position = glm::vec3(_mesh->mVertices[i].x,
                                     _mesh->mVertices[i].y,
                                     _mesh->mVertices[i].z);
        _vertex.normal = glm::vec3(_mesh->mNormals[i].x,
                                   _mesh->mNormals[i].y,
                                   _mesh->mNormals[i].z);

        if (_mesh->mTextureCoords[0])
        {
            _vertex.textureCoordinates = glm::vec2(
                _mesh->mTextureCoords[0][i].x,
                _mesh->mTextureCoords[0][i].y);
        }
        else
        {
            _vertex.textureCoordinates = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(_vertex);
    }

    for (unsigned int i = 0; i < _mesh->mNumFaces; ++i)
    {
        aiFace face = _mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    if (_mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = _scene->mMaterials[_mesh->mMaterialIndex];

        std::vector<texture> diffuseMaps = loadMaterialTextures(material,
                                                                aiTextureType_DIFFUSE, "diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<texture> specularMaps = loadMaterialTextures(material,
                                                                 aiTextureType_SPECULAR, "specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return mesh(vertices, indices, textures);
}
std::vector<gfx::texture> gfx::model::loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                                           std::string typeName)
{
    std::vector<texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool loaded = false;
        for (unsigned int j = 0; j < loaded_textures.size(); ++j)
        {
            if (std::strcmp(loaded_textures[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(loaded_textures[j]);
                loaded = true;
                break;
            }
        }

        if (!loaded)
        {
            texture _texture;
            _texture.id = loadTexture(str.C_Str(), directory);
            _texture.type = typeName;
            _texture.path = str.C_Str();
            textures.push_back(_texture);
            loaded_textures.push_back(_texture);
        }
    }
    return textures;
}
