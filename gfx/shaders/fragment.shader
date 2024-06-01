#version 330 core
out vec4 fragColor;

// in vec3 outColor;
in vec2 texcoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixamount;

void main()
{
    fragColor = mix(texture(texture1, texcoord), texture(texture2, texcoord), mixamount);
};