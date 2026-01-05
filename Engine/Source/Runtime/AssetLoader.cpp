#include "AssetLoader.h"
#include "IO/FileStream.h"

namespace Nova
{
    bool AssetLoader::FilepathIsValid(const StringView filepath) const
    {
        const Array<String> supportedExtensions = SupportedExtensions();
        return supportedExtensions.Any([&filepath](const String& extension)
        {
            return filepath.EndsWith(extension);
        });
    }

    Ref<Asset> AssetLoader::LoadFromFile(StringView filepath)
    {
        if (!FilepathIsValid(filepath)) return nullptr;

        FileStream stream(filepath, OpenModeFlagBits::ReadBinary);
        if (!stream.IsOpened()) return nullptr;
        return LoadFromStream(stream);
    }
}
