#pragma once
#include "Containers/String.h"
#include "AudioSettings.h"
#include "Rendering/GraphicsSettings.h"
#include "Filesystem.h"

namespace Nova
{
    struct ApplicationConfiguration
    {
        String AppName;
        Path IconPath;
        u32 WindowWidth = 0;
        u32 WindowHeight = 0;
        bool StartFullscreen = false;
        bool WindowResizable = false;
        bool WithEditor = false;
        GraphicsSettings Graphics;
        AudioSettings Audio;
    };
}
