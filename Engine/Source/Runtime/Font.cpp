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

    Font::~Font()
    {
        delete m_AtlasTexture;
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

        m_FontData.FontGeometry = msdf_atlas::FontGeometry(&m_FontData.Glyphs);


        msdf_atlas::Charset CharacterSet;
        for (const CharacterSetRange& Range : Params.CharacterSetRanges)
            for (u32 CodePoint = Range.Begin; CodePoint <= Range.End; CodePoint++)
                CharacterSet.add(CodePoint);

        const i32 LoadedGlyphs = m_FontData.FontGeometry.loadCharset(Font, 1.0, CharacterSet);
        if (LoadedGlyphs <= 0)
        {
            NOVA_LOG(Font, Verbosity::Error, "No glyphs loaded from: {}", Filepath.string());
            return false;
        }
        if (LoadedGlyphs != CharacterSet.size())
        {
            NOVA_LOG(Font, Verbosity::Warning, "{} glyphs loaded. Expected {}.", LoadedGlyphs,
                     msdf_atlas::Charset::ASCII.size());
        } else
        {
            NOVA_LOG(Font, Verbosity::Info, "All {} glyphs loaded.", LoadedGlyphs, CharacterSet.size());
        }


        //TODO: Need to create a function that generate atlas based on specified parameters
        constexpr double MaxCornerAngle = 3.0;
        for (msdf_atlas::GlyphGeometry& Glyph : m_FontData.Glyphs)
            Glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, MaxCornerAngle, 0);

        msdf_atlas::TightAtlasPacker Packer;
        Packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::POWER_OF_TWO_SQUARE);
        Packer.setDimensions(1024, 1024);
        Packer.setPixelRange(2.0);
        Packer.setMiterLimit(1.0);
        Packer.pack(m_FontData.Glyphs.data(), m_FontData.Glyphs.size());
        int Width = 0, Height = 0;
        Packer.getDimensions(Width, Height);

        Renderer* Renderer = g_Application->GetRenderer();

        switch (Params.AtlasType)
        {
        case FontAtlasType::SDF:
            {
                msdf_atlas::ImmediateAtlasGenerator<float, 1, msdf_atlas::sdfGenerator, msdf_atlas::BitmapAtlasStorage<u8, 1>> Generator(Width, Height);
                msdf_atlas::GeneratorAttributes Attributes;
                Attributes.scanlinePass = true;
                Generator.setAttributes(Attributes);
                Generator.setThreadCount(8);
                Generator.generate(m_FontData.Glyphs.data(), m_FontData.Glyphs.size());

                const msdfgen::BitmapConstRef<u8, 1>& Bitmap = Generator.atlasStorage();

                if (!m_AtlasTexture)
                {
                    const TextureParams TextureParams { Filter::Linear, SamplerAddressMode::Repeat, Format::R8_UNORM };
                    m_AtlasTexture = Texture2D::Create("Font", Width, Height, TextureParams, 0, Renderer->GetGraphicsApi());
                    if (!m_AtlasTexture)
                        return false;

                    const std::shared_ptr<Image> AtlasImage = std::make_shared<Image>(Width, Height, Format::R8_UNORM, Bitmap.pixels);
                    m_AtlasTexture->SetData(AtlasImage);
                }
            }
            break;
        case FontAtlasType::PSDF:
            {
                msdf_atlas::ImmediateAtlasGenerator<float, 1, msdf_atlas::psdfGenerator, msdf_atlas::BitmapAtlasStorage<u8, 1>> Generator(Width, Height);
                msdf_atlas::GeneratorAttributes Attributes;
                Attributes.scanlinePass = true;
                Generator.setAttributes(Attributes);
                Generator.setThreadCount(8);
                Generator.generate(m_FontData.Glyphs.data(), m_FontData.Glyphs.size());

                const msdfgen::BitmapConstRef<u8, 1>& Bitmap = Generator.atlasStorage();

                if (!m_AtlasTexture)
                {
                    const TextureParams TextureParams { Filter::Linear, SamplerAddressMode::Repeat, Format::R8_UNORM };
                    m_AtlasTexture = Texture2D::Create("Font", Width, Height, TextureParams, 0, Renderer->GetGraphicsApi());
                    if (!m_AtlasTexture)
                        return false;

                    const std::shared_ptr<Image> AtlasImage = std::make_shared<Image>(Width, Height, Format::R8_UNORM, Bitmap.pixels);
                    m_AtlasTexture->SetData(AtlasImage);
                }
            }
            break;
        case FontAtlasType::MSDF:
            {
                msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<u8, 3>> Generator(Width, Height);
                msdf_atlas::GeneratorAttributes Attributes;
                Attributes.scanlinePass = true;
                Generator.setAttributes(Attributes);
                Generator.setThreadCount(8);
                Generator.generate(m_FontData.Glyphs.data(), m_FontData.Glyphs.size());

                const msdfgen::BitmapConstRef<u8, 3>& Bitmap = Generator.atlasStorage();

                if (!m_AtlasTexture)
                {
                    const TextureParams TextureParams { Filter::Linear, SamplerAddressMode::Repeat, Format::R8G8B8_UNORM };
                    m_AtlasTexture = Texture2D::Create("Font", Width, Height, TextureParams, 0, Renderer->GetGraphicsApi());
                    if (!m_AtlasTexture)
                        return false;

                    const std::shared_ptr<Image> AtlasImage = std::make_shared<Image>(Width, Height, Format::R8G8B8_UNORM, Bitmap.pixels);
                    m_AtlasTexture->SetData(AtlasImage);
                }
            }
            break;
        case FontAtlasType::MTSDF:
            {
                msdf_atlas::ImmediateAtlasGenerator<float, 4, msdf_atlas::mtsdfGenerator, msdf_atlas::BitmapAtlasStorage<u8, 4>> Generator(Width, Height);
                msdf_atlas::GeneratorAttributes Attributes;
                Attributes.config.errorCorrection.mode = msdfgen::ErrorCorrectionConfig::INDISCRIMINATE;
                Attributes.config.errorCorrection.distanceCheckMode = msdfgen::ErrorCorrectionConfig::ALWAYS_CHECK_DISTANCE;
                Attributes.scanlinePass = true;
                Generator.setAttributes(Attributes);
                Generator.setThreadCount(8);
                Generator.generate(m_FontData.Glyphs.data(), m_FontData.Glyphs.size());

                const msdfgen::BitmapConstRef<u8, 4>& Bitmap = Generator.atlasStorage();

                if (!m_AtlasTexture)
                {
                    const TextureParams TextureParams { Filter::Linear, SamplerAddressMode::Repeat, Format::R8G8B8A8_UNORM };
                    m_AtlasTexture = Texture2D::Create("Font", Width, Height, TextureParams, 0, Renderer->GetGraphicsApi());
                    if (!m_AtlasTexture)
                        return false;

                    const std::shared_ptr<Image> AtlasImage = std::make_shared<Image>(Width, Height, Format::R8G8B8A8_UNORM, Bitmap.pixels);
                    m_AtlasTexture->SetData(AtlasImage);
                }
            }
            break;
        }

        msdfgen::destroyFont(Font);
        msdfgen::deinitializeFreetype(Freetype);
        return true;
    }

    const Texture2D* Font::GetAtlasTexture() const
    {
        return m_AtlasTexture;
    }

    const FontData& Font::GetFontData() const
    {
        return m_FontData;
    }
}
