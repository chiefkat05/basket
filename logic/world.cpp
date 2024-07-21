#include "collision.h" // includes world.h
#include <algorithm>

// oct_tree tree(0.0f, 0.0f, 0.0f, 400.0f, 400.0f, 400.0f);

void world::PlaceObject(std::string modelDir, std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
                        glm::vec2 tScale, bool obtainable, bool collidable, bool solid)
{
    for (unsigned int i = 0; i < models.size(); ++i)
    {
        if (models[i].directory == modelDir)
        {
            object tmp = tmp.create(position, scale, rotation, tScale, obtainable, collidable, solid);
            tmp.modelID = i;
            tmp.internalScale = glm::vec3(1.0f);
            objects.push_back(tmp);

            return;
        }
    }
    // gfx::model nModel(modelPath);
    gfx::mesh nModel(modelDir, modelPath);

    models.push_back(nModel);

    object tmp = tmp.create(position, scale, rotation, tScale, obtainable, collidable, solid);
    tmp.modelID = models.size() - 1;
    tmp.internalScale = glm::vec3(1.0f);

    objects.push_back(tmp);
    // tree.insert(objects.size() - 1);
}
void world::AddObject(object &obj, std::string modelDir, std::string modelPath)
{
    bool modelFound = false;
    for (unsigned int i = 0; i < models.size(); ++i)
    {
        if (models[i].directory == modelPath)
        {
            obj.modelID = i;
            modelFound = true;
            break;
        }
    }

    if (!modelFound)
    {
        gfx::mesh nModel(modelDir, modelPath);

        models.push_back(nModel);

        obj.modelID = models.size() - 1;
    }

    objects.push_back(obj);
    // tree.insert(objects.size() - 1);
}

glm::vec3 world::FurthestPoint(glm::vec3 direction, std::vector<gfx::vertex> &vertices, glm::vec3 objPosition, glm::vec3 objScale, glm::vec3 objRotation)
{ // implement rotation please
    glm::vec3 maxPoint;
    float maxDistance = -FLT_MAX;

    for (unsigned int i = 0; i < vertices.size(); ++i)
    {

        glm::vec3 checkPosition = ((vertices[i].position) * objScale) + objPosition;
        float distance = glm::dot(checkPosition, direction);
        if (distance > maxDistance)
        {
            maxDistance = distance;
            maxPoint = checkPosition;
        }
    }

    return maxPoint;
}
glm::vec3 world::GJK_Support(glm::vec3 direction, object &obj1, object &obj2)
{
    // rotation add here
    // return FurthestPoint(direction, models[obj1.modelID].meshes[obj1.collisionMeshID].vertices, obj1.position) -
    //        FurthestPoint(-direction, models[obj2.modelID].meshes[obj2.collisionMeshID].vertices, obj2.position);

    return FurthestPoint(direction, models[obj1.modelID].vertices, obj1.position, obj1.scale, obj1.rotation) -
           FurthestPoint(-direction, models[obj2.modelID].vertices, obj2.position, obj2.scale, obj2.rotation);
}
bool SameDirection(const glm::vec3 &direction, const glm::vec3 &ao)
{
    return glm::dot(direction, ao) > 0;
}
bool world::GJK_Line(simplex &points, glm::vec3 &direction)
{
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];

    glm::vec3 diff = b - a;
    glm::vec3 ao = -a;

    if (SameDirection(diff, ao))
    {
        direction = glm::cross(glm::cross(diff, ao), ao);
    }
    else
    {
        points = {a};
        direction = ao;
    }

    return false;
}
bool world::GJK_Triangle(simplex &points, glm::vec3 &direction)
{
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];
    glm::vec3 c = points[2];

    glm::vec3 diffAB = b - a;
    glm::vec3 diffAC = c - a;
    glm::vec3 ao = -a;

    glm::vec3 abc = glm::cross(diffAB, diffAC);

    if (SameDirection(glm::cross(abc, diffAC), ao))
    {
        if (SameDirection(diffAC, ao))
        {
            points = {a, c};
            direction = glm::cross(glm::cross(diffAC, ao), diffAC);
        }
        else
        {
            return GJK_Line(points = {a, b}, direction);
        }
    }
    else
    {
        if (SameDirection(glm::cross(diffAB, abc), ao))
        {
            return GJK_Line(points = {a, b}, direction);
        }
        else
        {
            if (SameDirection(abc, ao))
            {
                direction = abc;
            }
            else
            {
                points = {a, c, b};
                direction = -abc;
            }
        }
    }

    return false;
}
bool world::GJK_Tetrahedron(simplex &points, glm::vec3 &direction)
{
    glm::vec3 a = points[0];
    glm::vec3 b = points[1];
    glm::vec3 c = points[2];
    glm::vec3 d = points[3];

    glm::vec3 diffAB = b - a;
    glm::vec3 diffAC = c - a;
    glm::vec3 diffAD = d - a;
    glm::vec3 ao = -a;

    glm::vec3 abc = glm::cross(diffAB, diffAC);
    glm::vec3 acd = glm::cross(diffAC, diffAD);
    glm::vec3 adb = glm::cross(diffAD, diffAB);

    if (SameDirection(abc, ao))
    {
        return GJK_Triangle(points = {a, b, c}, direction);
    }
    if (SameDirection(acd, ao))
    {
        return GJK_Triangle(points = {a, c, d}, direction);
    }
    if (SameDirection(adb, ao))
    {
        return GJK_Triangle(points = {a, d, b}, direction);
    }

    return true;
}

std::pair<std::vector<glm::vec4>, size_t> world::EPA_GetFaceNormals(const std::vector<glm::vec3> &polytope, const std::vector<size_t> &faces)
{
    std::vector<glm::vec4> normals;
    size_t minTriangle = 0;
    float minDistance = FLT_MAX;

    for (size_t i = 0; i < faces.size(); i += 3)
    {
        glm::vec3 a = polytope[faces[i]];
        glm::vec3 b = polytope[faces[i + 1]];
        glm::vec3 c = polytope[faces[i + 2]];

        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
        float distance = glm::dot(normal, a);

        if (distance < 0)
        {
            normal *= -1;
            distance *= -1;
        }

        normals.emplace_back(normal, distance);

        if (distance < minDistance)
        {
            minTriangle = i / 3;
            minDistance = distance;
        }
    }

    return {normals, minTriangle};
}
void world::EPA_AddIfUniqueEdge(std::vector<std::pair<size_t, size_t>> &edges, const std::vector<size_t> &faces, size_t a, size_t b)
{
    auto reverse = std::find(edges.begin(), edges.end(), std::make_pair(faces[b], faces[a]));

    if (reverse != edges.end())
    {
        edges.erase(reverse);
    }
    else
    {
        edges.emplace_back(faces[a], faces[b]);
    }
}
CollisionData world::EPA(const simplex &simplex, object &obj1, object &obj2)
{
    std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());
    std::vector<size_t> faces = {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2};

    auto [normals, minFace] = EPA_GetFaceNormals(polytope, faces);

    glm::vec3 minNormal;
    float minDistance = FLT_MAX;

    while (minDistance == FLT_MAX)
    {
        minNormal = glm::vec3(normals[minFace].x, normals[minFace].y, normals[minFace].z);
        minDistance = normals[minFace].w;

        glm::vec3 support = GJK_Support(minNormal, obj1, obj2);
        float sDistance = glm::dot(minNormal, support);

        if (abs(sDistance - minDistance) > 0.001f)
        {
            minDistance = FLT_MAX;

            std::vector<std::pair<size_t, size_t>> unique_edges;

            for (size_t i = 0; i < normals.size(); i++)
            {
                // if (SameDirection(normals[i], support))
                if (glm::dot(glm::vec3(normals[i].x, normals[i].y, normals[i].z), support) > glm::dot(glm::vec3(normals[i].x, normals[i].y, normals[i].z), polytope[faces[i * 3]]))
                {
                    size_t f = i * 3;

                    EPA_AddIfUniqueEdge(unique_edges, faces, f, f + 1);
                    EPA_AddIfUniqueEdge(unique_edges, faces, f + 1, f + 2);
                    EPA_AddIfUniqueEdge(unique_edges, faces, f + 2, f);

                    faces[f + 2] = faces.back();
                    faces.pop_back();
                    faces[f + 1] = faces.back();
                    faces.pop_back();
                    faces[f] = faces.back();
                    faces.pop_back();

                    normals[i] = normals.back();
                    normals.pop_back();

                    i--;
                }
            }

            std::vector<size_t> newFaces;
            for (auto [edgeIndex1, edgeIndex2] : unique_edges)
            {
                newFaces.push_back(edgeIndex1);
                newFaces.push_back(edgeIndex2);
                newFaces.push_back(polytope.size());
            }

            polytope.push_back(support);

            auto [newNormals, newMinFace] = EPA_GetFaceNormals(polytope, newFaces);

            float oldMinDistance = FLT_MAX;
            for (size_t i = 0; i < normals.size(); ++i)
            {
                if (normals[i].w < oldMinDistance)
                {
                    oldMinDistance = normals[i].w;
                    minFace = i;
                }
            }

            if (newNormals[newMinFace].w < oldMinDistance)
            {
                minFace = newMinFace + normals.size();
            }

            faces.insert(faces.end(), newFaces.begin(), newFaces.end());
            normals.insert(normals.end(), newNormals.begin(), newNormals.end());
        }
    }

    CollisionData data;

    data.normal = minNormal;
    data.penetrationDepth = minDistance + 0.001f;
    data.hasCollision = true;

    return data;
}

bool world::collisionDetection(object &obj1, object &obj2)
{
    // rudimentary collision
    float distance = glm::distance(obj1.position, obj2.position);

    float rad1 = glm::length(obj1.scale) * models[obj1.modelID].boundingSphereRadius;
    float rad2 = glm::length(obj2.scale) * models[obj2.modelID].boundingSphereRadius;

    if (distance > rad1 * rad1 + rad2 * rad2)
        return false;

    glm::vec3 support = GJK_Support(glm::vec3(1.0f, 0.0f, 0.0f), obj1, obj2);

    simplex points;
    points.push_front(support);

    glm::vec3 direction = -support;

    // this tutorial is glorious
    while (true)
    {
        support = GJK_Support(direction, obj1, obj2);

        if (glm::dot(support, direction) <= 0.0f)
        {
            return false;
        }

        points.push_front(support);

        if (GJK_NextSimplex(points, direction))
        {
            obj1.colliding = true;
            obj2.colliding = true;
            // collision response
            CollisionData col = EPA(points, obj1, obj2);

            // add broadphase as a simple pre-collision test

            if (col.normal.y > 0.7f && obj2.velocity.y <= 0.0f)
                obj2.onGround = true;
            if (col.normal.y < -0.7f && obj1.velocity.y <= 0.0f)
                obj1.onGround = true;

            if (!obj1.solid)
            {
                obj1.position -= glm::vec3(col.normal.x, col.normal.y, col.normal.z) * col.penetrationDepth;
            }
            if (!obj2.solid)
            {
                obj2.position += glm::vec3(col.normal.x, col.normal.y, col.normal.z) * col.penetrationDepth;
            }
            return true;
        }
    }

    return false;
}

void world::Render(Shader &s, glm::mat4 &projection, glm::mat4 &view, glm::vec3 &playerPos, glm::vec3 &camFront, object *&objectLookingAt, object *&objectHolding,
                   unsigned int &objectHoldingID, float floorlevel, float delta_time)
{
    objectLookingAt = nullptr;
    s.use();
    s.setMat4("projection", projection); // collision mesh does not get rotated!!! // okay yea EPA is absolutely broken // also add quadtree
    s.setMat4("view", view);

    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objectHolding == &objects[i])
            objectHoldingID = i;
        if (objectHolding == nullptr)
            objectHoldingID = -1;
        if (!objects[i].solid)
        {
            // gravity
            if (objects[i].beingHeld == -1 || !objects[i].obtainable)
            {
                objects[i].velocity.y -= 30.0f * delta_time;
            }
            // if (objects[i].prevPosition.y == objects[i].position.y)
            //     objects[i].velocity.y = 0.0f;

            // player pickup
            if (objects[i].obtainable && objectHolding == nullptr)
            {
                glm::vec3 toObjectVector = playerPos - objects[i].position;

                glm::vec3 distance = glm::cross(camFront, toObjectVector);
                if (distance.y < 0.1f && distance.x < 0.1f && distance.z < 0.1f && toObjectVector.x < 3.0f && toObjectVector.y < 3.0f && toObjectVector.z < 3.0f)
                {
                    objects[i].internalScale = glm::vec3(0.87f);
                    s.setVec4("colorMultiple", glm::vec4(0.4f, 0.7f, 0.6f, 1.0f));
                    objectLookingAt = &objects[i];
                }
                else
                {
                    objects[i].internalScale = glm::vec3(1.0f);
                }
            }

            objects[i].position += objects[i].velocity * delta_time;

            if (objects[i].onGround)
            {
                objects[i].velocity.y = 0.0f;
            }
            objects[i].velocity.x *= 0.05f;
            objects[i].velocity.z *= 0.05f;

            objects[i].onGround = false;
        }

        // collision
        if (objects[i].collidable)
        {
            for (unsigned int j = 0; j < objects.size(); ++j)
            {
                if (j == i || !objects[j].collidable)
                    continue;

                if (collisionDetection(objects[i], objects[j]))
                {
                    // s.setVec4("colorMultiple", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
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
        s.setVec4("colorMultiple", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        objects[i].prevPosition = objects[i].position;
        objects[i].colliding = false;
    }
}