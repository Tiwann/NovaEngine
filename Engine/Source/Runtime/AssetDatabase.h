#pragma once
#include "Object.h"
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
        AssetType* CreateAsset(const String& name)
        {
            AssetType* asset = new AssetType();
            m_Data[name] = asset;
            return asset;
        }

        bool UnloadAsset(const String& name);
        bool UnloadAsset(Asset* asset);
        void UnloadAll();

        template<typename AssetType> requires std::is_base_of_v<Asset, AssetType>
        AssetType* Get(const String& name) const
        {
            if (!m_Data.Contains(name))
                return nullptr;
            return dynamic_cast<AssetType*>(m_Data[name]);
        }
    
    private:
        Map<String, Asset*> m_Data;
    };

    
}
