#pragma once
#include "../IO/Stream.h"
#include "TextureImporter.h"

namespace Nova
{


    
    class DDSImporter final : public TextureImporter
    {
    public:
        bool LoadFromFile(StringView filepath) override;
        bool LoadFromStream(Stream& stream) override;
        Array<String> SupportedExtensions() const override;
        Ref<Texture> CreateTexture() const override;
    private:
        DDSHeader m_Header{};
        DDSHeaderDXT10 m_HeaderDXT10{};
    };
}