#pragma once
#include "Containers/StringView.h"
#include "Runtime/Uuid.h"
#include "Runtime/Version.h"
#include "Runtime/Ref.h"
#include <cstdint>

namespace Nova
{
    class Asset;

    class AssetPack
    {
    public:
        Ref<Asset> GetAssetByName(StringView name);
        Ref<Asset> GetAssetByPath(StringView path);
        Ref<Asset> GetAssetByUuid(Uuid uuid);

        template<typename AssetType> requires IsBaseOfValue<Asset, AssetType>
        Ref<AssetType> GetAssetByName(StringView name);

        template<typename AssetType> requires IsBaseOfValue<Asset, AssetType>
        Ref<AssetType> GetAssetByPath(StringView path);

        template<typename AssetType> requires IsBaseOfValue<Asset, AssetType>
        Ref<AssetType> GetAssetByUuid(Uuid uuid);
    };
}
