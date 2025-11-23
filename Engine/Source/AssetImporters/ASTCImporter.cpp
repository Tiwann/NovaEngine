#include "ASTCImporter.h"
#include "IO/FileStream.h"
#include "Runtime/Application.h"
#include "Runtime/FileUtils.h"

namespace Nova
{
    ASTCImporter::ASTCImporter(ASTCFormat importFormat) : TextureImporter(), m_ImportFormat(importFormat)
    {

    }

    bool ASTCImporter::LoadFromFile(StringView filepath)
    {
        if (!FilepathIsValid(filepath)) return false;
        FileStream stream(filepath, OpenModeFlagBits::ReadBinary);
        return LoadFromStream(stream);
    }

    constexpr uint16_t MakeBlock(const uint8_t blockX, const uint8_t blockY)
    {
        return (blockX << 4) | blockY;
    }

    static Format GetFormatFromHeader(const ASTCFormat fmt, const ASTCHeader& inHeader)
    {
        const uint8_t blockX = inHeader.blockX;
        const uint8_t blockY = inHeader.blockY;

        switch (fmt)
        {
        case ASTCFormat::LDR:
            switch (MakeBlock(blockX, blockY))
            {
            case MakeBlock(4, 4): return Format::ASTC_4X4_UNORM;
            case MakeBlock(5, 4): return Format::ASTC_5X4_UNORM;
            case MakeBlock(5, 5): return Format::ASTC_5X5_UNORM;
            case MakeBlock(6, 5): return Format::ASTC_6X5_UNORM;
            case MakeBlock(6, 6): return Format::ASTC_6X6_UNORM;
            case MakeBlock(8, 5): return Format::ASTC_8X5_UNORM;
            case MakeBlock(8, 6): return Format::ASTC_8X6_UNORM;
            case MakeBlock(8, 8): return Format::ASTC_8X8_UNORM;
            case MakeBlock(10, 5): return Format::ASTC_10X5_UNORM;
            case MakeBlock(10, 6): return Format::ASTC_10X6_UNORM;
            case MakeBlock(10, 8): return Format::ASTC_10X8_UNORM;
            case MakeBlock(10, 10): return Format::ASTC_10X10_UNORM;
            case MakeBlock(12, 10): return Format::ASTC_12X10_UNORM;
            case MakeBlock(12, 12): return Format::ASTC_12X12_UNORM;
            default: break;
            }
        case ASTCFormat::HDR:
            switch (MakeBlock(blockX, blockY))
            {
            case MakeBlock(4, 4): return Format::ASTC_4X4_FLOAT;
            case MakeBlock(5, 4): return Format::ASTC_5X4_FLOAT;
            case MakeBlock(5, 5): return Format::ASTC_5X5_FLOAT;
            case MakeBlock(6, 5): return Format::ASTC_6X5_FLOAT;
            case MakeBlock(6, 6): return Format::ASTC_6X6_FLOAT;
            case MakeBlock(8, 5): return Format::ASTC_8X5_FLOAT;
            case MakeBlock(8, 6): return Format::ASTC_8X6_FLOAT;
            case MakeBlock(8, 8): return Format::ASTC_8X8_FLOAT;
            case MakeBlock(10, 5): return Format::ASTC_10X5_FLOAT;
            case MakeBlock(10, 6): return Format::ASTC_10X6_FLOAT;
            case MakeBlock(10, 8): return Format::ASTC_10X8_FLOAT;
            case MakeBlock(10, 10): return Format::ASTC_10X10_FLOAT;
            case MakeBlock(12, 10): return Format::ASTC_12X10_FLOAT;
            case MakeBlock(12, 12): return Format::ASTC_12X12_FLOAT;
            default: break;
            }
        case ASTCFormat::SRGB:
            switch (MakeBlock(blockX, blockY))
            {
            case MakeBlock(4, 4): return Format::ASTC_4X4_SRGB;
            case MakeBlock(5, 4): return Format::ASTC_5X4_SRGB;
            case MakeBlock(5, 5): return Format::ASTC_5X5_SRGB;
            case MakeBlock(6, 5): return Format::ASTC_6X5_SRGB;
            case MakeBlock(6, 6): return Format::ASTC_6X6_SRGB;
            case MakeBlock(8, 5): return Format::ASTC_8X5_SRGB;
            case MakeBlock(8, 6): return Format::ASTC_8X6_SRGB;
            case MakeBlock(8, 8): return Format::ASTC_8X8_SRGB;
            case MakeBlock(10, 5): return Format::ASTC_10X5_SRGB;
            case MakeBlock(10, 6): return Format::ASTC_10X6_SRGB;
            case MakeBlock(10, 8): return Format::ASTC_10X8_SRGB;
            case MakeBlock(10, 10): return Format::ASTC_10X10_SRGB;
            case MakeBlock(12, 10): return Format::ASTC_12X10_SRGB;
            case MakeBlock(12, 12): return Format::ASTC_12X12_SRGB;
            default: break;
            }
        }

        return Format::None;
    };

    bool ASTCImporter::LoadFromStream(Stream& stream)
    {
        ASTCHeader header;
        if (stream.ReadObject(header) != sizeof(ASTCHeader))
            return false;

        constexpr uint32_t astcMagic = 0x5CA1AB13;
        if (!Memory::Memcmp(&header.magic, &astcMagic, 4))
            return false;

        if (header.blockZ != 1) return false;

        const uint32_t width = header.dimX[0] + (header.dimX[1] << 8) + (header.dimX[2] << 16);
        const uint32_t height = header.dimY[0] + (header.dimY[1] << 8) + (header.dimY[2] << 16);
        const uint32_t depth = header.dimZ[0] + (header.dimZ[1] << 8) + (header.dimZ[2] << 16);

        const Array<uint8_t> compressedBlocks = FileUtils::ReadTillEnd(stream);

        TextureCreateInfo textureCreateInfo = TextureCreateInfo()
        .WithWidth(width)
        .WithHeight(height)
        .WithDepth(depth)
        .WithFormat(GetFormatFromHeader(m_ImportFormat, header))
        .WithFlags(TextureUsageFlagBits::Sampled)
        .WithMips(1)
        .WithSampleCount(1)
        .WithData(compressedBlocks);

        Application& application = Application::GetCurrentApplication();
        Ref<Device> device = application.GetDevice();
        Ref<Texture> texture = device->CreateTexture(textureCreateInfo);

        return true;
    }

    Array<String> ASTCImporter::SupportedExtensions() const
    {
        return { "astc" };
    }

    Ref<TextureImporter::Texture> ASTCImporter::CreateTexture() const
    {
        return nullptr;
    }
}
