#pragma once
#include <cstdint>

namespace Nova
{
    struct SpriteSheet
    {
        uint32_t rows;
        uint32_t columns;
        uint32_t spriteCount;
        uint32_t spriteSize;
    };
}
