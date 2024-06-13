#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include "../nms/glad/include/glad/gl.h"
#include <GLFW/glfw3.h>

struct Shader
{
    unsigned int ID;

    Shader(const std::string vPath, const std::string fPath);

    void use();

    void setBool(const std::string name, bool value) const;
    void setDouble(const std::string name, double value) const;
    void setInt(const std::string name, int value) const;

    void setVec2(const std::string name, glm::vec2 value) const;
    void setVec2(const std::string name, double x, double y) const;
    void setVec3(const std::string name, glm::vec3 value) const;
    void setVec3(const std::string name, double x, double y, double z) const;
    void setVec4(const std::string name, glm::vec4 value) const;
    void setVec4(const std::string name, double x, double y, double z, double w) const;

    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};

#endif