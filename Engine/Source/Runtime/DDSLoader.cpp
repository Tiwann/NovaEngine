#include "DDSLoader.h"

#include "Application.h"
#include "FileUtils.h"
#include "Rendering/Texture.h"

namespace Nova
{
    static constexpr uint32_t DDS_MAGIC = 0x20534444;

    static Format ConvertFormat(DDS_FORMAT format)
    {
        switch (format)
    {
        // =========================
        // 8-bit formats
        // =========================
        case DDS_FORMAT_R8_UNORM: return Format::R8_UNORM;
        case DDS_FORMAT_R8_SNORM: return Format::R8_SNORM;

        // =========================
        // 16-bit formats
        // =========================
        case DDS_FORMAT_R16_UNORM: return Format::R16_USHORT;
        case DDS_FORMAT_R16_SNORM: return Format::R16_SHORT;

        // =========================
        // 32-bit single channel
        // =========================
        case DDS_FORMAT_R32_FLOAT: return Format::R32_FLOAT;
        case DDS_FORMAT_R32_UINT:  return Format::R32_UINT;
        case DDS_FORMAT_R32_SINT:  return Format::R32_SINT;

        // =========================
        // RG formats
        // =========================
        case DDS_FORMAT_R8G8_UNORM: return Format::R8G8_UNORM;
        case DDS_FORMAT_R8G8_SNORM: return Format::R8G8_SNORM;

        case DDS_FORMAT_R16G16_UNORM: return Format::R16G16_USHORT;
        case DDS_FORMAT_R16G16_SNORM: return Format::R16G16_SHORT;

        case DDS_FORMAT_R32G32_UINT:  return Format::R32G32_UINT;
        case DDS_FORMAT_R32G32_SINT:  return Format::R32G32_SINT;
        case DDS_FORMAT_R32G32_FLOAT: return Format::R32G32_FLOAT;

        // =========================
        // RGB formats
        // =========================
        case DDS_FORMAT_R32G32B32_UINT:  return Format::R32G32B32_UINT;
        case DDS_FORMAT_R32G32B32_SINT:  return Format::R32G32B32_SINT;
        case DDS_FORMAT_R32G32B32_FLOAT: return Format::R32G32B32_FLOAT;

        // =========================
        // RGBA formats
        // =========================
        case DDS_FORMAT_R8G8B8A8_UNORM:       return Format::R8G8B8A8_UNORM;
        case DDS_FORMAT_R8G8B8A8_SNORM:       return Format::R8G8B8A8_SNORM;
        case DDS_FORMAT_R8G8B8A8_UNORM_SRGB:  return Format::R8G8B8A8_SRGB;

        case DDS_FORMAT_R16G16B16A16_UNORM: return Format::R16G16B16A16_USHORT;
        case DDS_FORMAT_R16G16B16A16_SNORM: return Format::R16G16B16A16_SHORT;

        case DDS_FORMAT_R32G32B32A32_UINT:  return Format::R32G32B32A32_UINT;
        case DDS_FORMAT_R32G32B32A32_SINT:  return Format::R32G32B32A32_SINT;
        case DDS_FORMAT_R32G32B32A32_FLOAT: return Format::R32G32B32A32_FLOAT;

        // =========================
        // Depth formats
        // =========================
        case DDS_FORMAT_D32_FLOAT:            return Format::D32_FLOAT;
        case DDS_FORMAT_D32_FLOAT_S8X24_UINT: return Format::D32_FLOAT_S8_UINT;
        case DDS_FORMAT_D24_UNORM_S8_UINT:    return Format::D24_UNORM_S8_UINT;

        // =========================
        // BC compressed formats
        // =========================
        case DDS_FORMAT_BC1_UNORM:       return Format::BC1_UNORM;
        case DDS_FORMAT_BC1_UNORM_SRGB:  return Format::BC1_SRGB;

        case DDS_FORMAT_BC2_UNORM:       return Format::BC2_UNORM;
        case DDS_FORMAT_BC2_UNORM_SRGB:  return Format::BC2_SRGB;

        case DDS_FORMAT_BC3_UNORM:       return Format::BC3_UNORM;
        case DDS_FORMAT_BC3_UNORM_SRGB:  return Format::BC3_SRGB;

        case DDS_FORMAT_BC4_UNORM:       return Format::BC4_UNORM;
        case DDS_FORMAT_BC4_SNORM:       return Format::BC4_SNORM;

        case DDS_FORMAT_BC5_UNORM:       return Format::BC5_UNORM;
        case DDS_FORMAT_BC5_SNORM:       return Format::BC5_SNORM;

        case DDS_FORMAT_BC7_UNORM:       return Format::BC7_UNORM;
        case DDS_FORMAT_BC7_UNORM_SRGB:  return Format::BC7_SRGB;

        // =========================
        // Unsupported / typeless / video
        // =========================
        default:
            return Format::None;
    }
    }

    Ref<Asset> DDSLoader::LoadFromStream(Stream& stream)
    {
        if (!stream.Rewind()) return nullptr;

        uint32_t ddsMagic = 0;
        stream.Read(ddsMagic);
        if (ddsMagic != DDS_MAGIC)
            return nullptr;

        DDSHeader header;
        if (stream.ReadObject(header) != sizeof(DDSHeader))
            return nullptr;

        DDSHeaderDXT10 headerDXT10;
        if (header.pixelFormat.flags & DDSPixelFormatFlagBits::FourCC)
        {
            const auto& fourCC = header.pixelFormat.fourCC;
            if (Memory::Memcmp(fourCC, "DX10", 4))
                stream.ReadObject(headerDXT10);
        }

        const Array<uint8_t> data = FileUtils::ReadTillEnd(stream);

        const TextureCreateInfo createInfo = TextureCreateInfo()
        .WithWidth(header.width)
        .WithHeight(header.height)
        .WithDepth(header.depth)
        .WithMips(header.mipCount)
        .WithFormat(ConvertFormat(headerDXT10.format))
        .WithData(data);

        const Application& application = Application::GetCurrentApplication();
        Ref<Device> device = application.GetDevice();
        return device->CreateTexture(createInfo);
    }

    Array<String> DDSLoader::SupportedExtensions() const
    {
        return { ".dds" };
    }
}
