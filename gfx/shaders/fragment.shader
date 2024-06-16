#version 330 core

in vec2 texcoord;

uniform sampler2D image;
uniform vec3 color;

void main()
{
    gl_FragColor = texture(image, texcoord).rgb * color;
};