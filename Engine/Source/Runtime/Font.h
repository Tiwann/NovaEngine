#pragma once
#include "Asset.h"
#include "Filesystem.h"

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
    
    struct FontParams
    {
        FontAtlasType AtlasType;
        
    };

    
    class Font : public Asset
    {
    public:
        Font(const String& Name);
        ~Font() override = default;

        String GetAssetType() const override;
        bool LoadFromFile(const Path& Filepath);
    private:
        Texture2D* m_FontAtlas = nullptr;
    };
}