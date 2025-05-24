#pragma once
#include "Runtime/Asset.h"
#include "Runtime/Filesystem.h"
#include "Containers/Array.h"
#include "Serialization/FileStream.h"

// AssetPackage binary format (
namespace Nova
{
    class AssetPackage : public Asset
    {
    public:
        explicit AssetPackage(const String& Name);
        bool Serialize(const Path& Filepath);
        void Deserialize(const Path& Filepath);
    private:
        Array<Asset*> m_Assets;
        FileStream m_Stream;
    };
}
