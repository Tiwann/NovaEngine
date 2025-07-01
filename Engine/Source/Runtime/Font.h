#pragma once
#include "Asset.h"
#include "Filesystem.h"
#include "Containers/ArrayView.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Font, "FONT")

namespace msdf_atlas
{
    class FontGeometry;
    class GlyphGeometry;
}

namespace Nova
{
    class Texture2D;


    enum class FontAtlasType
    {
        SDF,
        PSDF,
        MSDF,
        MTSDF,
    };

    struct CharacterSetRange
    {
        u32 Begin = 0, End = 0;
    };
    
    struct FontParams
    {
        FontAtlasType AtlasType;
        ArrayView<CharacterSetRange> CharacterSetRanges;
    };


    struct FontData
    {
        std::shared_ptr<std::vector<msdf_atlas::GlyphGeometry>> GlyphGeometry = nullptr;
        std::shared_ptr<msdf_atlas::FontGeometry> FontGeometry = nullptr;
    };

    
    class Font : public Asset
    {
    public:
        explicit Font(const String& Name);
        ~Font() override;

        String GetAssetType() const override;
        bool LoadFromFile(const Path& Filepath, const FontParams& Params);

        const Texture2D* GetAtlasTexture() const;

        const FontData& GetFontData() const;
    private:
        Texture2D* m_AtlasTexture = nullptr;
        FontData m_FontData;
    };
}