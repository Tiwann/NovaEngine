#pragma once
#include "Runtime/Flags.h"

namespace Nova
{
    enum class TextureUsageFlagBits
    {
        None = 0,
        Sampled = BIT(0),
        Storage = BIT(1),
        Transfer = BIT(2),
    };

    using TextureUsageFlags = Flags<TextureUsageFlagBits>;
}