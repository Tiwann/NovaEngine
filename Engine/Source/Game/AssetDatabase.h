#pragma once
#include "Runtime/Object.h"
#include "Containers/Map.h"
#include <type_traits>

namespace Nova
{
    class Asset;
    class Sound;
    
    class AssetDatabase : public Object
    {
    public:
        AssetDatabase() : Object("Asset Database"){}
        ~AssetDatabase() override = default;

        template<typename AssetType, typename... Args> requires std::is_base_of_v<Asset, AssetType>
        AssetType* CreateAsset(const String& name, Args&&... arguments)
        {
            AssetType* asset = new AssetType(name, std::forward<Args>(arguments)...);
            m_Data[name] = asset;
            return asset;
        }

        bool UnloadAsset(const String& name);
        bool UnloadAsset(Asset* asset);
        void UnloadAll();

        template<typename AssetType> requires std::is_base_of_v<Asset, AssetType>
        AssetType* Get(const String& name)
        {
            if (!m_Data.Contains(name))
                return nullptr;
            return dynamic_cast<AssetType*>(m_Data[name]);
        }
    
    private:
        Map<String, Asset*> m_Data;
    };

    
}
