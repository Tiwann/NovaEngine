#pragma once
#include "TextureImporter.h"

namespace Nova
{
    struct ASTCHeader
    {
        struct uint24_t{ uint8_t values[3]{}; uint8_t operator[](int i) const {return values[i];} };

        uint8_t magic[4] = { 'A', 'S', 'T', 'C' };
        uint8_t blockX = 0;
        uint8_t blockY = 0;
        uint8_t blockZ = 0;
        uint24_t dimX = {};
        uint24_t dimY = {};
        uint24_t dimZ = {};
    };

    static_assert(sizeof(ASTCHeader) == 16, "ASTCHeader should be 16 bytes");

    enum class ASTCFormat
    {
        LDR,
        HDR,
        SRGB
    };

    class ASTCImporter final : public TextureImporter
    {
    public:
        ASTCImporter(ASTCFormat importFormat = ASTCFormat::HDR);
        ~ASTCImporter() override = default;
        bool LoadFromFile(StringView filepath) override;
        bool LoadFromStream(Stream& stream) override;
        Array<String> SupportedExtensions() const override;
        Ref<Texture> CreateTexture() const override;
    private:
        ASTCFormat m_ImportFormat;
    };
}
