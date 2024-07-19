#include "collision.h"

// declare empty node
// oct_tree::oct_tree(world *_level, int _objID)
// {
//     level = _level;
//     objID = _objID;
// }

// // declare root node
// oct_tree::oct_tree(float _x, float _y, float _z, float _x2, float _y2, float _z2)
// {
//     if (_x > _x2 || _y > _y2 || _z > _z2)
//     {
//         std::cout << "\n\toctree invalid bounds defined\n";
//         return;
//     }

//     // just in case you get confused again, CHIEF, this little line of code is
//     // what causes the current tree to be branded as a parent node.
//     objID = -1;
//     // ^ that line of code. The one right there.

//     topleftfront = glm::vec3(_x, _y, _z);
//     bottomrightback = glm::vec3(_x2, _y2, _z2);

//     children.assign(8, nullptr);
//     for (int i = TopLeftFront; i < BottomLeftBack; ++i)
//     {
//         children[i] = new oct_tree();
//     }
// }

// void oct_tree::insert(int _objID)
// {
//     if (find(_objID))
//     {
//         std::cout << "point already exists\n";
//         return;
//     }

//     if (level->objects[_objID].position.x < topleftfront.x || level->objects[_objID].position.x > bottomrightback.x || level->objects[_objID].position.y < topleftfront.y || level->objects[_objID].position.y > bottomrightback.y ||
//         level->objects[_objID].position.z < topleftfront.z || level->objects[_objID].position.z > bottomrightback.z)
//     {
//         std::cout << "point out of bounds\n";
//         return;
//     }

//     int midx = (topleftfront.x + bottomrightback.x) / 2;
//     int midy = (topleftfront.x + bottomrightback.x) / 2;
//     int midz = (topleftfront.x + bottomrightback.x) / 2;

//     int pos = -1;

//     if (level->objects[_objID].position.x <= midx)
//     {
//         if (level->objects[_objID].position.y <= midy)
//         {
//             if (level->objects[_objID].position.z <= midz)
//                 pos = TopLeftFront;
//             else
//                 pos = TopLeftBack;
//         }
//         else
//         {
//             if (level->objects[_objID].position.z <= midz)
//                 pos = BottomLeftFront;
//             else
//                 pos = BottomLeftBack;
//         }
//     }
//     else
//     {
//         if (level->objects[_objID].position.y <= midy)
//         {
//             if (level->objects[_objID].position.z <= midz)
//                 pos = TopRightFront;
//             else
//                 pos = TopRightBack;
//         }
//         else
//         {
//             if (level->objects[_objID].position.z <= midz)
//                 pos = BottomRightFront;
//             else
//                 pos = BottomRightBack;
//         }
//     }

//     // if child is occupied (if the child has children of it's own)
//     if (children[pos]->objID == -1)
//     {
//         children[pos]->insert(_objID);
//         return;
//     }
//     // if child is empty
//     else if (children[pos]->level == nullptr)
//     {
//         delete children[pos];
//         children[pos] = new oct_tree(level, _objID);
//         return;
//     }
//     // if this node is empty
//     else
//     {
//         unsigned int childObj = children[pos]->objID;

//         delete children[pos];
//         children[pos] = nullptr;
//         switch (pos)
//         {
//         case TopLeftFront:
//             children[pos] = new oct_tree(topleftfront.x,
//                                          topleftfront.y,
//                                          topleftfront.z,
//                                          midx,
//                                          midy,
//                                          midz);
//             break;
//         case TopRightFront:
//             children[pos] = new oct_tree(midx + 1,
//                                          topleftfront.y,
//                                          topleftfront.z,
//                                          bottomrightback.x,
//                                          midy,
//                                          midz);
//             break;
//         case BottomRightFront:
//             children[pos] = new oct_tree(midx + 1,
//                                          midy + 1,
//                                          topleftfront.z,
//                                          bottomrightback.x,
//                                          bottomrightback.y,
//                                          midz);
//             break;
//         case BottomLeftFront:
//             children[pos] = new oct_tree(topleftfront.x,
//                                          midy + 1,
//                                          topleftfront.z,
//                                          midx,
//                                          bottomrightback.y,
//                                          midz);
//             break;
//         case TopLeftBack:
//             children[pos] = new oct_tree(topleftfront.x,
//                                          topleftfront.y,
//                                          midz + 1,
//                                          midx,
//                                          midy,
//                                          bottomrightback.z);
//             break;
//         case TopRightBack:
//             children[pos] = new oct_tree(midx + 1,
//                                          topleftfront.y,
//                                          midz + 1,
//                                          bottomrightback.x,
//                                          midy,
//                                          bottomrightback.z);
//             break;
//         case BottomRightBack:
//             children[pos] = new oct_tree(midx + 1,
//                                          midy + 1,
//                                          midz + 1,
//                                          bottomrightback.x,
//                                          bottomrightback.y,
//                                          bottomrightback.z);
//             break;
//         case BottomLeftBack:
//             children[pos] = new oct_tree(topleftfront.x,
//                                          midy + 1,
//                                          midz + 1,
//                                          midx,
//                                          bottomrightback.y,
//                                          bottomrightback.z);
//             break;
//         default:
//             std::cout << "octree says - this should be impossible!\n";
//             return;
//         }
//         children[pos]->insert(childObj);
//         children[pos]->insert(_objID);
//     }
// }

// bool oct_tree::find(int _objID)
// {
//     if (level->objects[_objID].position.x < topleftfront.x ||
//         level->objects[_objID].position.x > bottomrightback.x ||
//         level->objects[_objID].position.y < topleftfront.y ||
//         level->objects[_objID].position.y > bottomrightback.y ||
//         level->objects[_objID].position.z < topleftfront.z ||
//         level->objects[_objID].position.z > bottomrightback.z)
//         return 0;

//     int midx = (topleftfront.x + bottomrightback.x) / 2;
//     int midy = (topleftfront.y + bottomrightback.y) / 2;
//     int midz = (topleftfront.z + bottomrightback.z) / 2;

//     int pos = -1;

//     if (level->objects[_objID].position.x <= midx)
//     {
//         if (level->objects[_objID].position.y <= midy)
//         {
//             if (level->objects[_objID].position.z <= midz)
//                 pos = TopLeftFront;
//             else
//                 pos = TopLeftBack;
//         }
//         else
//         {
//             if (level->objects[_objID].position.z <= midz)
//                 pos = BottomLeftFront;
//             else
//                 pos = BottomLeftBack;
//         }
//     }
//     else
//     {
//         if (level->objects[_objID].position.y <= midy)
//         {
//             if (level->objects[_objID].position.z <= midz)
//                 pos = TopRightFront;
//             else
//                 pos = TopRightBack;
//         }
//         else
//         {
//             if (level->objects[_objID].position.z <= midz)
//                 pos = BottomRightFront;
//             else
//                 pos = BottomRightBack;
//         }
//     }

//     // if the children have children tell them to look instead
//     if (children[pos]->objID == -1)
//         return children[pos]->find(_objID);
//     // if children are empty stop the search because you won't find anything
//     else if (children[pos]->level == nullptr)
//         return 0;
//     // if children have content then check if that content is the desired result
//     else
//     {
//         if (_objID == children[pos]->objID)
//             return 1;
//     }
//     // note that the above values are returns and not continues or something
//     // because the pos variable finds the location of the object's node

//     return 0;
// }

// bool oct_tree::Collisions()
// {
//     if ()

//     return false;
// }