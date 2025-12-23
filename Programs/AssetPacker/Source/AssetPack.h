#pragma once
#include "Containers/StringView.h"
#include "Containers/String.h"
#include "Runtime/AssetType.h"
#include "Runtime/UUID.h"
#include "Runtime/Ref.h"

#include <cstdint>

namespace Nova
{
    class Stream;
    class Asset;

    struct AssetEntry
    {
        String virtualPath;
        UUID uuid;
        AssetType assetType;
        uint64_t chunkOffset;
        uint64_t chunkSize;

        bool Read(Stream& stream);
    };

    class AssetPack
    {
    public:
        bool ReadFromFile(StringView filepath);

        Ref<Asset> LoadAssetFromVirtualPath(StringView virtualPath);
        Ref<Asset> LoadAssetFromUUID(UUID uuid);
        Ref<Asset> LoadAssetFromEntry(const AssetEntry& entry);

        template<typename T>
        Ref<T> LoadAssetFromVirtualPath(StringView virtualPath)
        {
            return LoadAssetFromVirtualPath(virtualPath).As<T>();
        }

        template<typename T>
        Ref<T> LoadAssetFromUUID(UUID uuid)
        {
            return LoadAssetFromUUID(uuid).As<T>();
        }

        template<typename T>
        Ref<T> LoadAssetFromEntry(const AssetEntry& entry)
        {
            return LoadAssetFromEntry(entry).As<T>();
        }

    private:
        Array<AssetEntry> m_AssetTable;
        Array<uint8_t> m_AssetBlob;
    };
}
