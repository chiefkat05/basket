#ifndef EVENTS_H
#define EVENTS_H

#include <iostream>
#include <deque>
#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

enum e_type
{
    NULL_EVENT,
    INPUT_EVENT,
    COLLISION_EVENT,
    MESSAGE_EVENT,
    POSITION_EVENT,
    VARIABLE_EVENT
};
struct event
{
    e_type type;
    std::string data;
};

class event_handler
{
public:
    void send(event ev);
    int requestKeyState(int key);
    event requestObjectState(int objID);
    event poll();
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

private:
    std::deque<event> queue;
    std::vector<int> keyStates; // 10000+keycode = held, 20000+keycode = the first frame pressed
};

#endif