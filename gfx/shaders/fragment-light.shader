#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;

uniform DirectionalLight dLight;

// uniform const int pointLightCount;
#define pointLightCount 4
uniform PointLight pLights[pointLightCount];

// uniform const int spotLightCount;
#define spotLightCount 1
uniform SpotLight sLights[spotLightCount];

uniform vec3 viewPos;

in vec2 TexCoords;
in vec3 fragmentPosition;
in vec3 normal;

vec3 dirLightCalculation(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;
    vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * light.specular;

    return ambient + diffuse + specular;
}
vec3 pointLightCalculation(PointLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragmentPosition);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;
    vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * light.specular;

    return (ambient + diffuse + specular) * attenuation;
}
vec3 spotLightCalculation(SpotLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragmentPosition);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;
    vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * light.specular;

    return (ambient + diffuse + specular) * intensity * attenuation;
}

void main()
{
    vec3 result;
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragmentPosition);


    result = dirLightCalculation(dLight, norm, viewDir);

    for (int i = 0; i < pointLightCount; ++i)
    {
        result += pointLightCalculation(pLights[i], norm, viewDir);
    }
    for (int i = 0; i < spotLightCount; ++i)
    {
        result += spotLightCalculation(sLights[i], norm, viewDir);
    }
    // diffuse.x = fragmentPosition.y * -0.9;
    // diffuse.y = fragmentPosition.y * -0.9;
    // diffuse.z = fragmentPosition.y * -0.9;

    // if (int(fragmentPosition.x * 20 + fragmentPosition.z * 20) % 3 == 1)
    // {
    //     diffuse = vec3((fragmentPosition.y + 1.0) * -0.5);
    // }
    // result = (((diffuse + specular) * intensity) + ambient + emission) * attenuation;
    // result = result + material.emission;

    gl_FragColor = vec4(result, 1.0);
};