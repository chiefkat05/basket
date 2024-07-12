#ifndef WORLD_H
#define WORLD_H

#include "../gfx/graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>

struct object
{
    size_t modelID = 0;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 prevPosition = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 internalScale = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec2 tScale = glm::vec2(1.0f);

    bool obtainable = false, collidable = false, solid = true;
    int beingHeld = -1;
    float weight = 9.81f;
    bool invisible = false, includesTransparency = false;

    glm::vec3 velocity = glm::vec3(0.0f);

    // collision stuff
    int collisionMeshID = 0;
};
struct simplex
{
private:
    std::array<glm::vec3, 4> m_points;
    unsigned m_size;

public:
    simplex() : m_points({glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)}), m_size(0) {}

    simplex &operator=(std::initializer_list<glm::vec3> list)
    {
        for (auto v = list.begin(); v != list.end(); ++v)
        {
            m_points[std::distance(list.begin(), v)] = *v;
        }
        m_size = list.size();

        return *this;
    }

    void push_front(glm::vec3 point)
    {
        m_points = {point, m_points[0], m_points[1], m_points[2]};
        m_size = std::min(m_size + 1, 4u);
    }

    glm::vec3 &operator[](unsigned i) { return m_points[i]; }
    unsigned size() const { return m_size; }

    auto begin() const { return m_points.begin(); }
    auto end() const { return m_points.end() - (4 - m_size); }
};

struct CollisionData
{
    glm::vec3 normal = glm::vec3(0.0f);
    float penetrationDepth = 0.0f;
    bool hasCollision = false;
};

struct world
{
    std::vector<object> objects;
    std::vector<gfx::model> models;

    void PlaceObject(std::string modelPath, glm::vec3 position = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(1.0f),
                     glm::vec2 tScale = glm::vec2(1.0f), bool obtainable = false,
                     bool collidable = false, bool solid = true, float weight = 9.81f);
    void AddObject(object &obj, std::string modelPath);
    void Render(Shader &s, glm::mat4 &projection, glm::mat4 &view, glm::vec3 &playerPos, glm::vec3 &camFront, object *&objectLookingAt, object *&objectHolding,
                unsigned int &objectHoldingID, float floorlevel, float delta_time);
    bool collisionDetection(object &obj1, object &obj2);
    glm::vec3 FurthestPoint(glm::vec3 direction, std::vector<gfx::vertex> &vertices, glm::vec3 objPosition);
    glm::vec3 GJK_Support(glm::vec3 direction, object &obj1, object &obj2);
    bool GJK_Line(simplex &points, glm::vec3 &direction);
    bool GJK_Triangle(simplex &points, glm::vec3 &direction);
    bool GJK_Tetrahedron(simplex &points, glm::vec3 &direction);

    bool GJK_NextSimplex(simplex &points, glm::vec3 &direction)
    {
        switch (points.size())
        {
        case 2:
            return GJK_Line(points, direction);
        case 3:
            return GJK_Triangle(points, direction);
        case 4:
            return GJK_Tetrahedron(points, direction);
        }

        return false;
    }

    std::pair<std::vector<glm::vec4>, size_t> EPA_GetFaceNormals(const std::vector<glm::vec3> &polytope, const std::vector<size_t> &faces);
    void EPA_AddIfUniqueEdge(std::vector<std::pair<size_t, size_t>> &edges, const std::vector<size_t> &faces, size_t a, size_t b);
    CollisionData EPA(const simplex &points, object &obj1, object &obj2);
};

#endif