#include "events.h"

event event_handler::poll()
{
    event polledEvent = {NULL_EVENT, ""};
    if (queue.empty())
        return polledEvent;

    polledEvent = queue.back();

    // actually do event stuff here
    switch (polledEvent.type)
    {
    case INPUT_EVENT:
        break;
    case COLLISION_EVENT:

        break;
    default:
        break;
    }

    queue.pop_back();
    return polledEvent;
}

void event_handler::send(event ev)
{
    queue.emplace_front(ev);
}
// 0 is unpressed, 1 is held, 2 is pressed last frame
int event_handler::requestKeyState(int key)
{
    if (keyStates.empty())
        return 0;

    for (unsigned int i = 0; i < keyStates.size(); ++i)
    {
        if (keyStates[i] == key + 10000)
        {
            return 1;
        }
        if (keyStates[i] == key + 20000)
        {
            keyStates[i] = key + 10000;
            return 2;
        }
    }

    return 0;
}

void event_handler::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    int droppedKeyIndex = -1;
    for (unsigned int i = 0; i < keyStates.size(); ++i)
    {
        if (keyStates[i] == key + 10000)
        {
            if (action == 0)
            {
                droppedKeyIndex = i;
                break;
            }
            return;
        }
    }

    if (droppedKeyIndex == -1)
    {
        keyStates.push_back(key + 20000);
        return;
    }

    for (unsigned int i = droppedKeyIndex; i < keyStates.size() - 1; ++i)
    {
        keyStates[i] = keyStates[i + 1];
    }
    keyStates.pop_back();
}

// onPress(KEY_A):
//      ++x;

// onHeld(KEY_S):
//      --y;