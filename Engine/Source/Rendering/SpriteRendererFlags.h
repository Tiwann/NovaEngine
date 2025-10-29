#pragma once
#include "Runtime/Flags.h"

namespace Nova
{
    enum class SpriteRendererFlagBits
    {
        None = 0,
        TileWithScale = BIT(0),
        FlipHorizontal = BIT(1),
        FlipVertical = BIT(2),
    };

    typedef Flags<SpriteRendererFlagBits> SpriteRendererFlags;
}
