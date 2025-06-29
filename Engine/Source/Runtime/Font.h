#pragma once
#include "Asset.h"
#include "Filesystem.h"
#include "Containers/ArrayView.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Font, "FONT")

namespace Nova
{
    class Texture2D;


    enum class FontAtlasType
    {
        Rasterized,
        SDF8,
        SDF16,
        SDF32,
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

    
    class Font : public Asset
    {
    public:
        explicit Font(const String& Name);
        ~Font() override = default;

        String GetAssetType() const override;
        bool LoadFromFile(const Path& Filepath, const FontParams& Params);
    private:
        Texture2D* m_AtlasTexture = nullptr;
    };
}