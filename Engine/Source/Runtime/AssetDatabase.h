#pragma once
#include "Asset.h"
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
            for (size_t index = 0; index < m_Data.Count(); index++)
            {
                auto& pair = m_Data.GetAt(index);
                if (!pair.value)
                {
                    pair.key = name;
                    pair.value = new AssetType();
                    pair.value->SetObjectName(name);
                    return pair.value;
                }
            }

            m_Data[name] = new AssetType();
            return Ref(m_Data[name]);
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
