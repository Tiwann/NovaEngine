#pragma once
#include "AssetPack.h"

namespace Nova
{
    template <typename AssetType> requires IsBaseOfValue<Asset, AssetType>
    Ref<AssetType> AssetPack::GetAssetByName(const StringView name)
    {
        return GetAssetByName(name).As<AssetType>();
    }

    template <typename AssetType> requires IsBaseOfValue<Asset, AssetType>
    Ref<AssetType> AssetPack::GetAssetByPath(const StringView path)
    {
        return GetAssetByPath(path).As<AssetType>();
    }

    template <typename AssetType> requires IsBaseOfValue<Asset, AssetType>
    Ref<AssetType> AssetPack::GetAssetByUuid(const UUID uuid)
    {
        return GetAssetByUuid(uuid).As<AssetType>();
    }
}