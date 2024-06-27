#ifndef WORLD_H
#define WORLD_H

#include "../gfx/graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct object
{
    gfx::model model;
    glm::vec3 position, scale, rotation;
    glm::vec2 tScale;
};

struct world
{
    std::vector<object> objects;
    std::vector<gfx::model> models;

    void PlaceObject(std::string modelPath, glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(1.0f),
                     glm::vec2 tScale = glm::vec2(1.0f))
    {
        for (unsigned int i = 0; i < models.size(); ++i)
        {
            if (models[i].fullPath == modelPath)
            {
                objects.push_back({models[i], position, scale, rotation, tScale});
            }
        }
        gfx::model nModel(modelPath);

        objects.push_back({nModel, position, scale, rotation, tScale});
    }
    void Render(Shader &s, glm::mat4 &projection, glm::mat4 &view)
    {
        s.use();
        s.setMat4("projection", projection);
        s.setMat4("view", view);
        for (unsigned int i = 0; i < objects.size(); ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, objects[i].position);
            model = glm::scale(model, objects[i].scale);
            model = glm::rotate(model, glm::radians(objects[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(objects[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(objects[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            s.setVec2("textureScale", objects[i].tScale);
            s.setMat4("model", model);
            objects[i].model.draw(s);
        }
    }
};

#endif