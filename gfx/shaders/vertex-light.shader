#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMat;
uniform bool speeding;

uniform vec2 textureScale;

out vec3 normal;
out vec3 fragmentPosition;
out vec2 TexCoords;

void main()
{
    int posY = 0;

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    if (speeding)
    {
        gl_Position.z *= 0.96;
        gl_Position.x *= 0.96;
    }
    fragmentPosition = vec3(model * vec4(aPos, 1.0));
    normal = normalMat * aNormal;

    TexCoords = aTexCoords * textureScale;
};