#include "world.h"

void world::PlaceObject(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
                        glm::vec2 tScale, glm::vec3 hitPos, glm::vec3 hitScale, bool obtainable, bool collidable, float weight)
{
    for (unsigned int i = 0; i < models.size(); ++i)
    {

        if (models[i].fullPath == modelPath)
        {
            objects.push_back({i, position, position, scale, glm::vec3(1.0f), rotation, tScale, hitPos, hitScale, obtainable, collidable, -1, weight});
            return;
        }
    }
    gfx::model nModel(modelPath);

    models.push_back(nModel);
    objects.push_back({models.size() - 1, position, position, scale, glm::vec3(1.0f), rotation, tScale, hitPos, hitScale, obtainable, collidable, -1, weight});
}
void world::Render(Shader &s, glm::mat4 &projection, glm::mat4 &view, glm::vec3 &playerPos, glm::vec3 &camFront, object *&objectLookingAt, object *&objectHolding,
                   unsigned int &objectHoldingID, float floorlevel, float delta_time)
{
    objectLookingAt = nullptr;
    s.use();
    s.setMat4("projection", projection);
    s.setMat4("view", view);
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objectHolding == &objects[i])
            objectHoldingID = i;
        if (objectHolding == nullptr)
            objectHoldingID = -1;
        if (objects[i].obtainable)
        {

            // rudimentary collision
            // for (unsigned int j = 0; j < objects.size(); ++j)
            // {
            //     if (j == i || !objects[j].collidable)
            //         continue;

            //     // collision time
            //     // std::vector<gfx::vertex> vertices = objects[j].model->meshes[0].vertices;
            // }

            // gravity
            if (objects[i].position.y > floorlevel && objects[i].beingHeld == -1)
            {
                objects[i].position.y -= objects[i].weight * delta_time;
            }
            if (objects[i].position.y < floorlevel)
                objects[i].position.y = floorlevel;

            // player pickup
            if (objectHolding == nullptr)
            {
                glm::vec3 toObjectVector = playerPos - objects[i].position;

                glm::vec3 distance = glm::cross(camFront, toObjectVector);
                if (distance.y < 0.1f && distance.x < 0.1f && distance.z < 0.1f && toObjectVector.x < 3.0f && toObjectVector.y < 3.0f && toObjectVector.z < 3.0f)
                {
                    objects[i].internalScale = glm::vec3(0.87f);
                    s.setVec3("colorMultiple", glm::vec3(0.4f, 0.7f, 0.6f));
                    objectLookingAt = &objects[i];
                }
                else
                {
                    objects[i].internalScale = glm::vec3(1.0f);
                }
            }
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, objects[i].position);
        model = glm::scale(model, objects[i].scale * objects[i].internalScale);
        model = glm::rotate(model, glm::radians(objects[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(objects[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(objects[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        s.setVec2("textureScale", objects[i].tScale);
        s.setMat4("model", model);
        models[objects[i].modelID].draw(s);
        s.setVec3("colorMultiple", glm::vec3(1.0f, 1.0f, 1.0f));

        objects[i].prevPosition = objects[i].position;
    }
}