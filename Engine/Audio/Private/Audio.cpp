#include "Audio.h"
#include <fmod/fmod.hpp>
#include <stdio.h>

bool Sound::LoadSound(const char* fileName)
{
    FMOD::Sound* LoadedSound = nullptr;
    FMOD::System* System = nullptr;
    if (System_Create(&System) != FMOD_OK)
    {
        printf("FMOD::System_Create() failed\n");
        return false;
    }

    System->init(32, FMOD_INIT_NORMAL, nullptr);
    System->createSound(fileName, FMOD_DEFAULT, nullptr, &LoadedSound);

    return true;
}
