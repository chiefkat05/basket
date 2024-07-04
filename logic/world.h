#ifndef WORLD_H
#define WORLD_H

#include "../gfx/graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct object
{
    size_t modelID = 0;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 prevPosition = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 internalScale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec2 tScale = glm::vec2(1.0f);

    bool obtainable = false, collidable = false;
    int beingHeld = -1;
    float weight = 9.81f;
    bool invisible = false;

    glm::vec3 velocity = glm::vec3(0.0f);

    // collision stuff
    int collisionMeshID = 0;
};

struct world
{
    std::vector<object> objects;
    std::vector<gfx::model> models;

    void PlaceObject(std::string modelPath, glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(1.0f),
                     glm::vec2 tScale = glm::vec2(1.0f), bool obtainable = false,
                     bool collidable = false, float weight = 9.81f);
    void AddObject(object &obj, std::string modelPath);
    void Render(Shader &s, glm::mat4 &projection, glm::mat4 &view, glm::vec3 &playerPos, glm::vec3 &camFront, object *&objectLookingAt, object *&objectHolding,
                unsigned int &objectHoldingID, float floorlevel, float delta_time);
    bool collisionDetection(object &obj1, object &obj2);
    glm::vec3 FurthestPoint(glm::vec3 direction, std::vector<gfx::vertex> &vertices);
};

#endif