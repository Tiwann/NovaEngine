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
            
        asset.Release();
        return true;
    }

    bool AssetDatabase::UnloadAsset(Ref<Asset> asset)
    {
        const size_t index = m_Data.FindValue(asset);
        if (index == ~0) return false;
        asset.Release();
        return true;
    }

    void AssetDatabase::UnloadAll()
    {
        for (auto& [name, asset] : m_Data)
        {
            asset.Release();
        }
        m_Data.Clear();
    }
}
