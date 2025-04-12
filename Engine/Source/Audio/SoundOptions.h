#pragma once
#include "Runtime/Flags.h"

namespace Nova
{
    class Sound;
    
    enum class SoundFlagBits
    {
        Default = 0x00000000,
        Sound2D = 0x00000008,
        Sound3D = 0x00000010,
        LoopOff = 0x00000001,
        LoopOn = 0x00000002,
        LoadStream = 0x00000080,
        LoadDefault = 0x00000100,
        LoadCompressed = 0x00000200,
        Unique = 0x00020000,
        CreateSample = 0x00000100,
        CreateStream = 0x00000080,
        OpenMemory = 0x00000800,
    };

    NOVA_DECLARE_FLAGS(SoundFlagBits, SoundFlags);
}
