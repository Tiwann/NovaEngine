#include "AssetPackage.h"
#include "Serialization/FileStream.h"

namespace Nova
{
    AssetPackage::AssetPackage(const String& Name) : Asset(Name)
    {

    }

    bool AssetPackage::Serialize(const Path& Filepath)
    {
        FileStream Stream(Filepath, OpenModeFlagBits::ReadBinary);
        if (!Stream.IsOpened())
            return false;

        return false;
    }

    void AssetPackage::Deserialize(const Path& Filepath)
    {
    }

    Asset* AssetPackage::GetAssetFromPath(const String& AssetPath)
    {
        return nullptr;
    }

    Asset* AssetPackage::GetAssetFromUUID(const UUID& AssetUUID)
    {
        return nullptr;
    }
}
