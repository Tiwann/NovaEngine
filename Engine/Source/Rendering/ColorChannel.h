#pragma once
#include "Runtime/Flags.h"

namespace Nova
{
    enum class ColorChannelFlagBits
    {
        None = 0,
        Red = BIT(0),
        Green = BIT(1),
        Blue = BIT(2),
        Alpha = BIT(3),
    };

    typedef Flags<ColorChannelFlagBits> ColorChannelFlags;
}
