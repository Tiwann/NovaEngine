#pragma once
#include "Runtime/Ref.h"
#include <cstdint>

namespace Nova
{
    class ITexture;

    struct SpriteSheet
    {
        uint32_t rows;
        uint32_t columns;
        uint32_t spriteCount;
        uint32_t spriteSize;
        Ref<ITexture> texture = nullptr;
    };
}
