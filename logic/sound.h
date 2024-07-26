#ifndef SOUND_H
#define SOUND_H

#define MINIAUDIO_IMPLEMENTATION
#include "../nms/miniaudio.h"

#include <iostream>

struct sound_system
{
    ma_result err;
    ma_engine engine;
    ma_sound_group group;

    sound_system()
    {
        err = ma_engine_init(NULL, &engine);

        if (err != MA_SUCCESS)
        {
            std::cout << "sound initialization error.\n";
            return;
        }

        err = ma_sound_group_init(&engine, 0, NULL, &group);

        if (err != MA_SUCCESS)
        {
            std::cout << "sound initialization error.\n";
            return;
        }
    }
    ~sound_system()
    {
        ma_engine_uninit(&engine);
    }

    void play(std::string path)
    {
        err = ma_engine_play_sound(&engine, path.c_str(), &group);

        if (err != MA_SUCCESS)
        {
            std::cout << "sound error: " << err << "\n";
            return;
        }
    }
    void loop(std::string path)
    {
        if (!ma_sound_group_is_playing(&group))
            err = ma_engine_play_sound(&engine, path.c_str(), &group);

        if (err != MA_SUCCESS)
        {
            std::cout << "sound error: " << err << "\n";
            return;
        }
    }
};

#endif