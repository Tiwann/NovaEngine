#include "AssetDatabase.h"

namespace Nova
{
    bool AssetDatabase::UnloadAsset(const String& Name)
    {
        if (!m_Data.Contains(Name))
            return false;
            
        Asset* Asset = m_Data[Name];
        if (!m_Data.Remove({ Name, Asset }))
            return false;
            
        delete Asset;
        return true;
    }

    void AssetDatabase::UnloadAll()
    {
        for (const Pair<String, Asset*>& Data : m_Data)
        {
            delete Data.Value;
        }
    }
}
