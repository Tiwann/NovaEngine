#pragma once
#include "Runtime/Ref.h"
#include <cstdint>

namespace Nova
{
    namespace Rendering { class Texture; }
    struct SpriteSheet
    {
        uint32_t rows;
        uint32_t columns;
        uint32_t spriteCount;
        uint32_t spriteSize;
        Ref<Rendering::Texture> texture = nullptr;
    };
}
