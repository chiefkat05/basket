#include "shader.h"

Shader::Shader(const std::string vPath, const std::string fPath)
{
    /** get shader source codes **/

    std::string vertexSource, fragmentSource;
    std::ifstream vertexFile(vPath), fragmentFile(fPath);

    if (!vertexFile.is_open())
        std::cerr << "vertex file failed to open" << std::endl;
    if (!fragmentFile.is_open())
        std::cerr << "fragment file failed to open" << std::endl;

    std::string tempLine;
    while (std::getline(vertexFile, tempLine))
    {
        vertexSource.append(tempLine + '\n');
    };
    while (std::getline(fragmentFile, tempLine))
    {
        fragmentSource.append(tempLine + '\n');
    };

    const char *vShaderCode = vertexSource.c_str();
    const char *fShaderCode = fragmentSource.c_str();

    /** create and link shader program **/

    unsigned int vShader, fShader;
    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vShader, 1, &vShaderCode, NULL);
    glShaderSource(fShader, 1, &fShaderCode, NULL);
    glCompileShader(vShader);
    glCompileShader(fShader);

    int success = false;
    char errLog[512];
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vShader, 512, NULL, errLog);
        std::cout << "vertex shader compilation error: " << errLog << std::endl;
        return;
    }
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fShader, 512, NULL, errLog);
        std::cout << "fragment shader compilation error: " << errLog << std::endl;
        return;
    }

    ID = glCreateProgram();

    glAttachShader(ID, vShader);
    glAttachShader(ID, fShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, errLog);
        std::cout << "shader program linking error: " << errLog << std::endl;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setDouble(const std::string name, double value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setInt(const std::string name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec2(const std::string name, glm::vec2 value) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}
void Shader::setVec2(const std::string name, double x, double y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}
void Shader::setVec3(const std::string name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}
void Shader::setVec3(const std::string name, double x, double y, double z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}
void Shader::setVec4(const std::string name, glm::vec4 value) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}
void Shader::setVec4(const std::string name, double x, double y, double z, double w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
