#include "Font.h"
#include "FileUtils.h"
#include "Application.h"
#include "Rendering/Texture.h"
#include "Log.h"
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include "Utils/TextureUtils.h"


namespace Nova
{
    bool Font::LoadFromFile(const StringView filepath, const FontParams& params)
    {
        Array<uint8_t> fileContent = FileUtils::ReadToBuffer(filepath);
        return LoadFromMemory(fileContent.Data(), fileContent.Size(), params);
    }

    bool Font::LoadFromMemory(const void* data, uint64_t dataSize, const FontParams& params)
    {
        if (!data || !dataSize) return false;

         msdfgen::FreetypeHandle* freetype = msdfgen::initializeFreetype();
        if (!freetype)
        {
            NOVA_LOG(Font, Verbosity::Error, "Failed to initialize Freetype!");
            return false;
        }

        msdfgen::FontHandle* font = msdfgen::loadFontData(freetype, (const msdfgen::byte*)data, dataSize);
        if (!font) return false;

        msdf_atlas::FontGeometry fontGeometry;
        msdf_atlas::Charset characterSet;
        for (const auto& [begin, end] : params.characterSetRanges)
            for (uint32_t codePoint = begin; codePoint <= end; codePoint++)
                characterSet.add(codePoint);

        const int32_t loadedGlyphs = fontGeometry.loadCharset(font, 1.0, characterSet);
        if (loadedGlyphs <= 0)
        {
            NOVA_LOG(Font, Verbosity::Error, "No glyphs loaded.");
            return false;
        }
        if (loadedGlyphs != characterSet.size())
        {
            NOVA_LOG(Font, Verbosity::Warning, "{} glyphs loaded. Expected {}.", loadedGlyphs, characterSet.size());
        } else
        {
            NOVA_LOG(Font, Verbosity::Info, "All {} glyphs loaded.", loadedGlyphs, characterSet.size());
        }


        //TODO: Need to create a function that generate atlas based on specified parameters
        constexpr int32_t dimension = 512, width = dimension, height = dimension;
        constexpr double maxCornerAngle = 3.0;
        msdf_atlas::FontGeometry::GlyphRange glyphs = fontGeometry.getGlyphs();
        for (const msdf_atlas::GlyphGeometry& glyph : glyphs)
            ((msdf_atlas::GlyphGeometry&)glyph).edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);

        msdf_atlas::TightAtlasPacker packer;
        packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::POWER_OF_TWO_SQUARE);
        packer.setDimensions(dimension, dimension);
        packer.setPixelRange(2.0);
        packer.setMiterLimit(1.0);
        if (packer.pack((msdf_atlas::GlyphGeometry*)glyphs.begin(), glyphs.size()) != 0)
            return false;

        Application& application = Application::GetCurrentApplication();
        Ref<RenderDevice> renderDevice = application.GetRenderDevice();

        switch (params.atlasType)
        {
        case FontAtlasType::SDF:
            {
                msdf_atlas::ImmediateAtlasGenerator<float, 1, msdf_atlas::sdfGenerator, msdf_atlas::BitmapAtlasStorage<uint8_t, 1>> generator(width, height);
                msdf_atlas::GeneratorAttributes attributes;
                attributes.scanlinePass = true;
                generator.setAttributes(attributes);
                generator.setThreadCount(8);
                generator.generate(glyphs.begin(), glyphs.size());

                const msdfgen::BitmapConstRef<uint8_t, 1>& bitmap = generator.atlasStorage();

                if (m_AtlasTexture) m_AtlasTexture->Destroy();
                TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8_UNORM);
                m_AtlasTexture = renderDevice->CreateTexture(createInfo);
                if (!m_AtlasTexture) return false;
                TextureUtils::UploadTextureData(renderDevice, m_AtlasTexture, 0, 0, bitmap.pixels, width * height);
            }
            break;
        case FontAtlasType::PSDF:
            {
                msdf_atlas::ImmediateAtlasGenerator<float, 1, msdf_atlas::psdfGenerator, msdf_atlas::BitmapAtlasStorage<uint8_t, 1>> generator(width, height);
                msdf_atlas::GeneratorAttributes attributes;
                attributes.scanlinePass = true;
                generator.setAttributes(attributes);
                generator.setThreadCount(8);
                generator.generate(glyphs.begin(), glyphs.size());
                const msdfgen::BitmapConstRef<uint8_t>& bitmap = generator.atlasStorage();

                if (m_AtlasTexture) m_AtlasTexture->Destroy();
                TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8_UNORM);
                m_AtlasTexture = renderDevice->CreateTexture(createInfo);
                if (!m_AtlasTexture) return false;
                TextureUtils::UploadTextureData(renderDevice, m_AtlasTexture, 0, 0, bitmap.pixels, width * height);
            }
            break;
        case FontAtlasType::MSDF:
            {
                msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<uint8_t, 3>> generator(width, height);
                msdf_atlas::GeneratorAttributes attributes;
                attributes.scanlinePass = true;
                generator.setAttributes(attributes);
                generator.setThreadCount(8);
                generator.generate(glyphs.begin(), glyphs.size());

                const msdfgen::BitmapConstRef<uint8_t, 3>& bitmap = generator.atlasStorage();

                if (m_AtlasTexture) m_AtlasTexture->Destroy();
                TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8G8B8_UNORM);
                m_AtlasTexture = renderDevice->CreateTexture(createInfo);
                if (!m_AtlasTexture) return false;
                TextureUtils::UploadTextureData(renderDevice, m_AtlasTexture, 0, 0, bitmap.pixels, width * height * 3);
            }
            break;
        case FontAtlasType::MTSDF:
            {
                msdf_atlas::ImmediateAtlasGenerator<float, 4, msdf_atlas::mtsdfGenerator, msdf_atlas::BitmapAtlasStorage<uint8_t, 4>> generator(width, height);
                msdf_atlas::GeneratorAttributes attributes;
                attributes.config.errorCorrection.mode = msdfgen::ErrorCorrectionConfig::INDISCRIMINATE;
                attributes.config.errorCorrection.distanceCheckMode = msdfgen::ErrorCorrectionConfig::ALWAYS_CHECK_DISTANCE;
                attributes.scanlinePass = true;
                generator.setAttributes(attributes);
                generator.setThreadCount(8);
                generator.generate(glyphs.begin(), glyphs.size());

                const msdfgen::BitmapConstRef<uint8_t, 4>& bitmap = generator.atlasStorage();
                if (m_AtlasTexture) m_AtlasTexture->Destroy();
                TextureCreateInfo createInfo = TextureCreateInfo::Texture2D(width, height, Format::R8G8B8A8_UNORM);
                m_AtlasTexture = renderDevice->CreateTexture(createInfo);
                if (!m_AtlasTexture) return false;
                TextureUtils::UploadTextureData(renderDevice, m_AtlasTexture, 0, 0, bitmap.pixels, width * height * 4);
            }
            break;
        }


        msdfgen::destroyFont(font);
        msdfgen::deinitializeFreetype(freetype);
        return true;
    }

    void Font::Destroy()
    {
        if (m_AtlasTexture) m_AtlasTexture->Destroy();
    }
}
