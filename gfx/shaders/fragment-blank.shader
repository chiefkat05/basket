#version 330 core

uniform vec3 inColor;

void main()
{
    gl_FragColor = vec4(inColor, 1.0);
};