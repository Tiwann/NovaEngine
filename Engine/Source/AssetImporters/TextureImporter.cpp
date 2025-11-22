#pragma once
#include "TextureImporter.h"

namespace Nova
{
    bool TextureImporter::FilepathIsValid(const StringView filepath) const
    {
        const Array<String> supportedExtensions = SupportedExtensions();
        return supportedExtensions.Any([&filepath](const String& extension)
        {
            return filepath.EndsWith(extension);
        });
    }
}
