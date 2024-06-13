#version 330 core

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 fragmentPosition;
in vec3 normal;

out vec4 fragColor;

void main()
{
    float ambientStr = 0.1;
    float specularStr = 0.9;
    vec3 ambient = ambientStr * lightColor;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragmentPosition);

    vec3 viewDir = normalize(viewPos - fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, norm);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    diffuse.x = fragmentPosition.y * -0.01;
    diffuse.y = fragmentPosition.y * -0.01;
    diffuse.z = fragmentPosition.y * -0.01;

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = specularStr * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;

    gl_FragColor = vec4(result, 1.0);
};