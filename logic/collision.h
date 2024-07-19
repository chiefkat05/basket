#ifndef COLLISION_H
#define COLLISION_H

#include <iostream>
#include <vector>
#include "world.h"

#define TopLeftFront 0
#define TopRightFront 1
#define BottomRightFront 2
#define BottomLeftFront 3
#define TopLeftBack 4
#define TopRightBack 5
#define BottomRightBack 6
#define BottomLeftBack 7

// struct oct_tree
// {
//     world *level = nullptr;
//     int objID = -1;
//     glm::vec3 topleftfront, bottomrightback;

//     std::vector<oct_tree *> children;

//     oct_tree() {}

//     oct_tree(world *_level, int _objID);

//     oct_tree(float _x, float _y, float _z, float _x2, float _y2, float _z2);

//     void insert(int _objID);

//     bool find(int _objID);

//     bool Collisions();
// };

const unsigned int max_tree_objects = 5;

struct oct_tree
{
    int objIDs[max_tree_objects] = {0, 0, 0, 0, 0};
    world *level = nullptr;

    oct_tree *children[BottomLeftBack];
    oct_tree *parent = nullptr;
};

#endif