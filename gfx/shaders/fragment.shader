#version 330 core

in vec2 texcoord;

uniform sampler2D image;
uniform vec3 color;

void main()
{
    gl_FragColor = vec4(texture(image, texcoord).rgb * color, 1.0);
};