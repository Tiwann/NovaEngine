#pragma once
#include "Runtime/Version.h"
#include <cstdint>

namespace Nova
{
    class AssetPack
    {
    public:
        struct Header
        {
            uint32_t magic;
            Version version;
            uint64_t fileSize;
        };
    };
}
