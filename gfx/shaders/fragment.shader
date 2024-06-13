#version 330 core
out vec4 fragColor;

in vec2 texcoord;

uniform sampler2D image;

vec4 lightColor = vec4(0.8f, 0.8f, 0.8f, 1.0f);

void main()
{
    fragColor = texture(image, texcoord) * lightColor;
};