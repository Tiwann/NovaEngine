#include "AssetDatabase.h"
#include "Asset.h"

namespace Nova
{
    bool AssetDatabase::UnloadAsset(const String& name)
    {
        if (!m_Data.Contains(name))
            return false;
            
        Ref<Asset> asset = m_Data[name];
        if (!m_Data.Remove({ name, asset }))
            return false;
            
        delete asset;
        return true;
    }

    bool AssetDatabase::UnloadAsset(Ref<Asset> asset)
    {
        const size_t index = m_Data.FindValue(asset);
        if (index == ~0) return false;
        m_Data.RemoveAt(index);
        delete asset;
        return true;
    }

    void AssetDatabase::UnloadAll()
    {
        for (const auto& [name, asset] : m_Data)
        {
            delete asset;
        }
        m_Data.Clear();
    }
}
