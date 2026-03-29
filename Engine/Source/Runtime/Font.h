#pragma once
#include "Asset.h"
#include "LogCategory.h"
#include "Ref.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Font, "FONT");

namespace Nova
{
    class Texture;

    enum class FontAtlasType
    {
        SDF,
        PSDF,
        MSDF,
        MTSDF,
    };

    struct CharacterSetRange
    {
        uint32_t begin = 0, end = 0;

        static const Array<CharacterSetRange>& Latin()
        {
            static Array<CharacterSetRange> ranges{{0x20, 0xFF}};
            return ranges;
        }
    };

    struct FontParams
    {
        FontAtlasType atlasType;
        BufferView<CharacterSetRange> characterSetRanges;
    };

    class Font : public Asset
    {
    public:
        bool LoadFromFile(StringView filepath, const FontParams& params);
        bool LoadFromMemory(const void* data, uint64_t dataSize, const FontParams& params);
        void Destroy() override;
        AssetType GetAssetType() const final { return AssetType::Font; }

    private:
        Ref<Texture> m_AtlasTexture = nullptr;
    };
}
