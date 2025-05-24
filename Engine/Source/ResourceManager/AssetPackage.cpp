#include "AssetPackage.h"

namespace Nova
{
    AssetPackage::AssetPackage(const String& Name) : Asset(Name)
    {
    }

    bool AssetPackage::Serialize(const Path& Filepath)
    {
        if (!m_Stream.Open(Filepath, OpenModeFlagBits::ReadBinary))
            return false;

        
    }

    void AssetPackage::Deserialize(const Path& Filepath)
    {
    }
}
