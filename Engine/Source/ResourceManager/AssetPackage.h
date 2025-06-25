#pragma once
#include "Runtime/Asset.h"
#include "Runtime/Filesystem.h"
#include "Containers/Array.h"


namespace Nova
{
    class AssetPackage : public Asset
    {
    public:
        explicit AssetPackage(const String& Name);
        bool Serialize(const Path& Filepath);
        void Deserialize(const Path& Filepath);

        Asset* GetAssetFromPath(const String& AssetPath);
        Asset* GetAssetFromUUID(const UUID& AssetUUID);

        template<typename AssetType> requires IsBaseOf<Asset, AssetType>::Value
        AssetType* GetAssetFromPath(const String& AssetPath)
        {
            return GetAssetFromPath(AssetPath)->As<AssetType>();
        }

        template<typename AssetType> requires IsBaseOf<Asset, AssetType>::Value
        AssetType* GetAssetFromUUID(const UUID& AssetUUID)
        {
            return GetAssetFromUUID(AssetUUID)->As<AssetType>();
        }

    private:
        Array<Asset*> m_Assets;
        ScopedBuffer<u8> m_Data;
    };
}
