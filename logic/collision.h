#ifndef COLLISION_H
#define COLLISION_H

#include <iostream>
#include <vector>

struct point
{
    double x, y, z;
    double velX, velY, velZ;
};

struct collider
{
    bool colliding = false;

    point _point;

    bool checkCollide(collider *other)
    {
        float distanceX = _point.x - other->_point.x;
        float distanceY = _point.y - other->_point.y;
        float distanceZ = _point.z - other->_point.z;

        if (distanceX * distanceX < 1.0f || distanceY * distanceY < 1.0f || distanceZ * distanceZ < 1.0f)
        {
            std::cout << "colliding!\n";
            _point.velX = 0.0f;
            _point.velY = 0.0f;
            _point.velZ = 0.0f;

            return true;
        }

        return false;
    }
};

#endif