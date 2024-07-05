#include "world.h"

void world::PlaceObject(std::string modelPath, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation,
                        glm::vec2 tScale, bool obtainable, bool collidable, bool solid, float weight)
{
    for (unsigned int i = 0; i < models.size(); ++i)
    {

        if (models[i].fullPath == modelPath)
        {
            objects.push_back({i, position, position, scale, glm::vec3(1.0f), rotation, tScale, obtainable, collidable, solid, -1, weight});
            return;
        }
    }
    gfx::model nModel(modelPath);

    models.push_back(nModel);
    objects.push_back({models.size() - 1, position, position, scale, glm::vec3(1.0f), rotation, tScale, obtainable, collidable, solid, -1, weight});
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

glm::vec3 world::FurthestPoint(glm::vec3 direction, std::vector<gfx::vertex> &vertices, glm::vec3 objPosition)
{
    glm::vec3 maxPoint;
    float maxDistance = -FLT_MAX;

    for (int i = 0; i < vertices.size(); ++i)
    {
        glm::vec3 checkPosition = vertices[i].position + objPosition;
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
    return FurthestPoint(direction, models[obj1.modelID].meshes[obj1.collisionMeshID].vertices, obj1.position) -
           FurthestPoint(-direction, models[obj2.modelID].meshes[obj2.collisionMeshID].vertices, obj2.position);
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
CollisionData world::EPA(const simplex &points, object &obj1, object &obj2)
{
    std::vector<glm::vec3> polytope(points.begin(), points.end());
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

            for (size_t i = 0; i < normals.size(); ++i)
            {
                if (!SameDirection(normals[i], support))
                    continue;

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

                --i;
            }

            std::vector<size_t> newFaces;
            for (auto [edgeIndex1, edgeIndex2] : unique_edges)
            {
                newFaces.push_back(edgeIndex1);
                newFaces.push_back(edgeIndex1);
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

    // std::vector<unsigned int> indices = models[obj2.modelID].meshes[obj2.collisionMeshID].indices;

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
            // collision
            CollisionData col = EPA(points, obj1, obj2);
            // if (!obj1.solid)

            // figure out why there is nan output sometimes (seems to have something to do with corners)
            // add broadphase as a simple pre-collision test
            if (!obj2.solid)
            {
                obj2.position += glm::vec3(col.normal.x, 0.0f, col.normal.z) * (col.penetrationDepth + 0.001f);
            }
            return true;
        }
    }

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