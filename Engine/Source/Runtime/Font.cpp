#include "Font.h"
#include "Rendering/Texture2D.h"

#include <msdfgen.h>
#include <msdfgen-ext.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>


#include "Application.h"
#include "Log.h"

namespace Nova
{
    Font::Font(const String& Name) : Asset(Name)
    {
    }

    String Font::GetAssetType() const
    {
        return "Font";
    }

    bool Font::LoadFromFile(const Path& Filepath, const FontParams& Params)
    {
        msdfgen::FreetypeHandle* Freetype = msdfgen::initializeFreetype();
        if (!Freetype)
        {
            NOVA_LOG(Font, Verbosity::Error, "Failed to initialize Freetype!");
            return false;
        }

        msdfgen::FontHandle* Font = msdfgen::loadFont(Freetype, Filepath.string().c_str());
        if (!Font)
        {
            NOVA_LOG(Font, Verbosity::Error, "Failed to load font {}", Filepath.string());
            return false;
        }

        std::vector<msdf_atlas::GlyphGeometry> Glyphs;
        msdf_atlas::FontGeometry FontGeometry(&Glyphs);

        /*msdf_atlas::Charset CharacterSet;
        for (const CharacterSetRange& Range : Params.CharacterSetRanges)
            for (u32 CodePoint = Range.Begin; CodePoint <= Range.End; CodePoint++)
                CharacterSet.add(CodePoint);*/

        const i32 LoadedGlyphs = FontGeometry.loadCharset(Font, 1.0, msdf_atlas::Charset::ASCII);
        if (LoadedGlyphs <= 0)
        {
            NOVA_LOG(Font, Verbosity::Error, "No glyphs loaded from: {}", Filepath.string());
            return false;
        }
        if (LoadedGlyphs != msdf_atlas::Charset::ASCII.size())
        {
            NOVA_LOG(Font, Verbosity::Warning, "{} glyphs loaded. Expected {}.", LoadedGlyphs,
                     msdf_atlas::Charset::ASCII.size());
        }
        NOVA_LOG(Font, Verbosity::Info, "All {} glyphs loaded.", LoadedGlyphs, msdf_atlas::Charset::ASCII.size());


        const double maxCornerAngle = 3.0;
        for (msdf_atlas::GlyphGeometry& glyph : Glyphs)
            glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);

        msdf_atlas::TightAtlasPacker packer;
        packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
        packer.setScale(48);
        packer.setPixelRange(2.0);
        packer.setMiterLimit(1.0);
        packer.pack(Glyphs.data(), Glyphs.size());
        // Get final atlas dimensions
        int width = 0, height = 0;
        packer.getDimensions(width, height);

        msdf_atlas::ImmediateAtlasGenerator<float,3,msdf_atlas::msdfGenerator,msdf_atlas::BitmapAtlasStorage<msdfgen::byte, 3>> generator(width, height);
        msdf_atlas::GeneratorAttributes attributes;
        generator.setAttributes(attributes);
        generator.setThreadCount(8);
        generator.generate(Glyphs.data(), Glyphs.size());

        msdfgen::saveBmp(generator.atlasStorage(), "atlas.bmp");


        Renderer* Renderer = g_Application->GetRenderer();
        if (!m_AtlasTexture)
        {
            m_AtlasTexture = Texture2D::Create("Font", Renderer->GetGraphicsApi());
            if (!m_AtlasTexture)
                return false;

            const std::shared_ptr<Image> AtlasImage = std::make_shared<Image>(width, height, Format::R8G8B8)
            m_AtlasTexture->SetData(std::make_shared<>())
        }
        return true;
    }
}
