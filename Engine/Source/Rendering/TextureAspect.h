#pragma once
#include "Runtime/Flags.h"

namespace Nova
{
    enum class TextureAspectFlagBits
    {
        None = 0,
        Color = BIT(0),
        Depth = BIT(1),
        Stencil = BIT(2),
    };

    using TextureAspectFlags = Flags<TextureAspectFlagBits>;
}
