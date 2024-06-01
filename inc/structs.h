#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>
#include <vector>

#define TEMP_DATA_LIMIT 128

struct obj
{
    std::string name;
    obj *parent;
    double x, y;
};

struct data
{
    const std::string file_path;
    unsigned char temporary_data[TEMP_DATA_LIMIT]; // make basket more stable and suitable for action games?
};

enum Event_Type
{
    E_COLLISION,
    E_PUSH,
    E_ADOPT,
    E_ORPHAN,
    E_SETPOS,
    E_DATA,
    E_VIS_ANIMATION,
    E_VIS_SHADER,
    E_VIS_OVERRIDE,
    E_HOSTSIDE_DATA,
    E_HOSTSIDE_VISUAL,
    E_CLIENTSIDE_DATA,
    E_CLIENTSIDE_VISUAL
};

struct event
{
    obj *a, *b;
    data *c;
    Event_Type type;
};

struct EventHandler
{
    std::vector<event> list, prev;
};

#endif