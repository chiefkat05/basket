#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

in vec2 TexCoords;
in vec3 fragmentPosition;
in vec3 normal;

void main()
{
    vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * light.ambient;

    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - fragmentPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;

    diffuse.x = fragmentPosition.y * -0.9;
    diffuse.y = fragmentPosition.y * -0.9;
    diffuse.z = fragmentPosition.y * -0.9;

    if (int(fragmentPosition.x * 20 + fragmentPosition.z * 20) % 3 == 1)
    {
        diffuse = vec3((fragmentPosition.y + 1.0) * -0.5);
    }

    vec3 viewDir = normalize(viewPos - fragmentPosition);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * light.specular;

    vec3 emission = vec3(texture(material.emission, TexCoords));

    gl_FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
};