#pragma once
#include "Runtime/Version.h"
#include "Containers/String.h"
#include "Runtime/AssetType.h"
#include "Runtime/UUID.h"

#include <cstdint>


namespace Nova
{
    class AssetPack
    {
    public:
        struct AssetEntry
        {
            String virtualPath;
            UUID uuid;
            AssetType assetType;
            uint64_t chunkOffset;
            uint64_t chunkSize;
        };

        struct Header
        {
            uint32_t magic;
            Version version;
            uint64_t fileSize;
        };
    };
}
