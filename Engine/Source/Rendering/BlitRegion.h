#pragma once
#include <cstdint>

namespace Nova
{
    struct BlitRegion
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t mipLevel = 0;
    };
}
