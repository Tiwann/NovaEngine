#pragma once
#include "../IO/Stream.h"
#include "Containers/StringView.h"
#include "Runtime/Ref.h"
#include "Rendering/Texture.h"

namespace Nova
{
    class TextureImporter
    {
    protected:
        using Texture = Rendering::Texture;

    public:
        TextureImporter() = default;
        virtual ~TextureImporter() = default;

        virtual bool LoadFromFile(StringView filepath) = 0;
        virtual bool LoadFromStream(Stream& stream) = 0;
        virtual Array<String> SupportedExtensions() const = 0;
        virtual Ref<Texture> CreateTexture() const = 0;

    protected:
        bool FilepathIsValid(StringView filepath) const;
    };
}
