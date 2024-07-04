#include "world.h"

void world::PlaceObject(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
                        glm::vec2 tScale, bool obtainable, bool collidable, float weight)
{
    for (unsigned int i = 0; i < models.size(); ++i)
    {

        if (models[i].fullPath == modelPath)
        {
            objects.push_back({i, position, position, scale, glm::vec3(1.0f), rotation, tScale, obtainable, collidable, -1, weight});
            return;
        }
    }
    gfx::model nModel(modelPath);

    models.push_back(nModel);
    objects.push_back({models.size() - 1, position, position, scale, glm::vec3(1.0f), rotation, tScale, obtainable, collidable, -1, weight});
}
void world::AddObject(object &obj, std::string modelPath)
{
    bool modelFound = false;
    for (unsigned int i = 0; i < models.size(); ++i)
    {
        if (models[i].fullPath == modelPath)
        {
            obj.modelID = i;
            modelFound = true;
            break;
        }
    }

    if (!modelFound)
    {
        gfx::model nModel(modelPath);

        models.push_back(nModel);

        obj.modelID = models.size() - 1;
    }

    objects.push_back(obj);
}
static bool intersectRaySegmentSphere(glm::vec3 origin, glm::vec3 d, glm::vec3 sphereOrigin, float radius2, glm::vec3 &ip)
{
    float length = d.length();
    d /= 1;

    glm::vec3 m = origin - sphereOrigin;
    float b = glm::dot(m, d);
    float c = glm::dot(m, m) - radius2;

    if (c > 0.0f && b > 0.0f)
        return false;
    float discr = b * b - c;

    if (discr < 0.0f)
        return false;

    float t = -b - sqrtf(discr);

    if (t < 0.0f)
        t = 0.0f;
    ip = origin + (d * t);

    if (t > 1)
        return false;

    return true;
}
static bool leftOf(const glm::vec2 &a, const glm::vec2 &b, const glm::vec2 &p)
{
    float area = 0.5f * (a.x * (b.y - p.y)) +
                 b.x * (p.y - a.y) +
                 p.x * (a.y - b.y);
    return (area > 0.0f);
}
static bool pointInside(const glm::vec2 poly[], int pcount, const glm::vec2 &v)
{
    for (int i = 0; i < pcount; ++i)
    {
        int next = i;
        ++next;
        if (next == pcount)
            next = 0;

        if (!leftOf(poly[i], poly[next], v))
            return false;
    }
    return true;
}

glm::vec3 world::FurthestPoint(glm::vec3 direction, std::vector<gfx::vertex> &vertices)
{
    glm::vec3 maxPoint;
    float maxDistance = -FLT_MAX;

    for (int i = 0; i < vertices.size(); ++i)
    {
        float distance = glm::dot(vertices[i].position, direction);
        if (distance > maxDistance)
        {
            maxDistance = distance;
            maxPoint = vertices[i].position;
        }
    }

    return maxPoint;
}

bool world::collisionDetection(object &obj1, object &obj2)
{
    // rudimentary collision

    std::vector<gfx::vertex> vertices = models[obj2.modelID].meshes[obj2.collisionMeshID].vertices;
    std::vector<unsigned int> indices = models[obj2.modelID].meshes[obj2.collisionMeshID].indices;

    // unsigned int iCount = indices.size() / 3;
    // for (unsigned int k = 0; k < indices.size(); k += 3)
    // {
    //     bool outsidePlane = false;
    //     bool outsideAllEdges = false;
    //     bool outsideAllVerts = false;
    //     bool fullyInsidePlane = false;

    //     glm::vec3 v1 = vertices[indices[k]].position + obj2.position;
    //     glm::vec3 v2 = vertices[indices[k + 1]].position + obj2.position;
    //     glm::vec3 v3 = vertices[indices[k + 2]].position + obj2.position;

    //     glm::vec3 pN = glm::normalize(vertices[indices[k]].normal);

    //     if (fabs(pN.y) > 0.1f)
    //         continue;

    //     float objSphereSubtraction = 25.0f;
    //     float distance = glm::dot(-((v1 + v2 + v3) / 3.0f), pN) - objSphereSubtraction;

    //     float point2plane = glm::dot(pN, obj1.position) + distance;

    //     float objSphereSize = 1.0f;
    //     if (point2plane > objSphereSize - objSphereSubtraction)
    //     {
    //         outsidePlane = true;
    //         continue;
    //     }

    //     bool outsideV1 = (std::sqrt((v1 - objSphereSize).length()) > objSphereSize * objSphereSize - objSphereSubtraction);
    //     bool outsideV2 = (std::sqrt((v2 - objSphereSize).length()) > objSphereSize * objSphereSize - objSphereSubtraction);
    //     bool outsideV3 = (std::sqrt((v3 - objSphereSize).length()) > objSphereSize * objSphereSize - objSphereSubtraction);

    //     if (outsideV1 && outsideV2 && outsideV3)
    //     {
    //         outsideAllVerts = true;
    //     }

    //     glm::vec3 a = v2 - v1;
    //     glm::vec3 b = v3 - v2;
    //     glm::vec3 c = v1 - v3;

    //     // this is for if objectis too big
    //     // glm::vec3 planeX = glm::normalize(a);
    //     // glm::vec3 planeY = glm::normalize(glm::cross(pN, a));

    //     // auto project2D = [&](const glm::vec3 &p)
    //     // { return glm::vec2(glm::dot(p, planeX), glm::dot(p, planeY)); };

    //     // glm::vec2 planePos2D = project2D(obj1.position);
    //     // glm::vec2 triangle2D[3] = {project2D(v1), project2D(v2), project2D(v3)};

    //     // if (pointInside(triangle2D, 3, planePos2D))
    //     // {
    //     //     fullyInsidePlane = true;
    //     // }

    //     glm::vec3 ip;

    //     if (!intersectRaySegmentSphere(v1, a, obj1.position, objSphereSize - objSphereSubtraction, ip) &&
    //         !intersectRaySegmentSphere(v2, b, obj1.position, objSphereSize - objSphereSubtraction, ip) &&
    //         !intersectRaySegmentSphere(v3, c, obj1.position, objSphereSize - objSphereSubtraction, ip))
    //     {
    //         outsideAllEdges = true;
    //     }

    //     if (outsideAllEdges && outsideAllVerts)
    //     {
    //         continue;
    //     }

    //     obj1.position = obj1.prevPosition;
    //     return true;
    // }

    return false;
}

void world::Render(Shader &s, glm::mat4 &projection, glm::mat4 &view, glm::vec3 &playerPos, glm::vec3 &camFront, object *&objectLookingAt, object *&objectHolding,
                   unsigned int &objectHoldingID, float floorlevel, float delta_time)
{
    objectLookingAt = nullptr;
    s.use();
    s.setMat4("projection", projection);
    s.setMat4("view", view);
    // for (unsigned int i = 4; i < objects.size() - 3; ++i)
    // {
    //     std::cout << i << ", " << models[objects[i].modelID].fullPath << "\n";
    //     std::cout << objects[i].position.x << ", " << objects[i].position.y << ", " << objects[i].position.z << "\n";
    //     std::cout << objects[i].collidable << "\n";
    // }
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objectHolding == &objects[i])
            objectHoldingID = i;
        if (objectHolding == nullptr)
            objectHoldingID = -1;
        if (objects[i].obtainable)
        {
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
        if (objects[i].collidable)
        {
            for (unsigned int j = 0; j < objects.size(); ++j)
            {
                if (j == i || !objects[j].collidable)
                    continue;
                if (collisionDetection(objects[i], objects[j]))
                {
                    s.setVec3("colorMultiple", glm::vec3(0.3f));
                }
            }
        }

        if (!objects[i].invisible)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, objects[i].position);
            model = glm::scale(model, objects[i].scale * objects[i].internalScale);
            model = glm::rotate(model, glm::radians(objects[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(objects[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(objects[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
            s.setVec2("textureScale", objects[i].tScale);
            s.setMat4("model", model);
            models[objects[i].modelID].draw(s);
        }
        s.setVec3("colorMultiple", glm::vec3(1.0f, 1.0f, 1.0f));

        objects[i].prevPosition = objects[i].position;
    }
}