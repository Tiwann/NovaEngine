#pragma once
#include "Object.h"
#include "Ref.h"
#include "Containers/Map.h"

namespace Nova
{
    class Asset;

    class AssetDatabase : public Object
    {
    public:
        AssetDatabase() : Object("Asset Database") {}
        ~AssetDatabase() override = default;

        template<typename AssetType> requires std::is_base_of_v<Asset, AssetType>
        Ref<AssetType> CreateAsset(const String& name)
        {
            Ref<AssetType> asset = Ref<AssetType>(new AssetType());
            m_Data[name] = asset;
            return asset;
        }

        template<typename AssetType> requires std::is_base_of_v<Asset, AssetType>
        void AddAsset(const Ref<AssetType>& asset, const String& name)
        {
            m_Data[name] = asset;
        }

        bool UnloadAsset(const String& name);
        bool UnloadAsset(Ref<Asset> asset);
        void UnloadAll();

        template<typename AssetType> requires std::is_base_of_v<Asset, AssetType>
        Ref<AssetType> Get(const String& name) const
        {
            if (!m_Data.Contains(name))
                return nullptr;
            return m_Data[name].As<AssetType>();
        }
    
    private:
        Map<String, Ref<Asset>> m_Data;
    };

    
}
