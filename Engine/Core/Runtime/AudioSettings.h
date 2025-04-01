#pragma once
#include "Types.h"
#include <CoreExport.h>

namespace Nova
{
    struct CORE_API AudioSettings
    {
        u32 SampleRate;
        u32 BufferSize;
        u32 BufferCount;
    };
}