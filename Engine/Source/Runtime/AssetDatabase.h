#pragma once
#include "Asset.h"
#include "LogCategory.h"
#include "Containers/Map.h"
#include <CoreExport.h>

NOVA_DECLARE_LOG_CATEGORY_STATIC(AssetDatabase, "ASSET DATABASE")

namespace Nova
{
    class Asset;
    class Sound;
    
    class CORE_API AssetDatabase : public Object
    {
    public:
        AssetDatabase() = default;
        ~AssetDatabase() override = default;

        template<typename AssetType, typename... Args> requires IsBaseOfValue<Asset, AssetType>
        AssetType* CreateAsset(const String& Name, Args&&... Arguments)
        {
            AssetType* Asset = new AssetType(Name, std::forward<Args>(Arguments)...);
            m_Data[Name] = Asset;
            return Asset;
        }

        bool UnloadAsset(const String& Name);
        void UnloadAll();

        template<typename AssetType> requires IsBaseOfValue<Asset, AssetType>
        AssetType* Get(const String& Name)
        {
            if (!m_Data.Contains(Name))
            {
                return nullptr;
            }
            return dynamic_cast<AssetType*>(m_Data[Name]);
        }
    
    private:
        Map<String, Asset*> m_Data;
    };

    
}
