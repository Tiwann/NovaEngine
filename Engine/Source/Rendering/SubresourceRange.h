#pragma once
#include <cstdint>

namespace Nova
{
    struct SubresourceRange
    {
        uint32_t baseMipLevel = 0;
        uint32_t mipCount = 0;
        uint32_t baseArrayLayer = 0;
        uint32_t layerCount = 0;
    };
}
