#ifndef WORLD_H
#define WORLD_H

#include "../gfx/graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct object
{
    size_t modelID;
    glm::vec3 position, prevPosition, scale, internalScale, rotation;
    glm::vec2 tScale;

    glm::vec3 hitboxPos, hitboxScale;
    bool obtainable, collidable;
    int beingHeld;
    float weight;
};

struct world
{
    std::vector<object> objects;
    std::vector<gfx::model> models;

    void PlaceObject(std::string modelPath, glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(1.0f),
                     glm::vec2 tScale = glm::vec2(1.0f), glm::vec3 hitPos = glm::vec3(0.0f), glm::vec3 hitScale = glm::vec3(1.0f), bool obtainable = false,
                     bool collidable = false, float weight = 9.81f);
    void Render(Shader &s, glm::mat4 &projection, glm::mat4 &view, glm::vec3 &playerPos, glm::vec3 &camFront, object *&objectLookingAt, object *&objectHolding,
                unsigned int &objectHoldingID, float floorlevel, float delta_time);
};

#endif