#pragma once
#include "../IO/Stream.h"
#include "TextureImporter.h"

namespace Nova
{
    enum class DDSPixelFormatFlagBits
    {
        AlphaPixels = 0x1,
        Alpha = 0x2,
        FourCC = 0x4,
        RGB = 0x40,
        YUV = 0x200,
        Luminance = 0x2000
    };

    NOVA_DECLARE_FLAGS(DDSPixelFormatFlagBits, DDSPixelFormatFlags);

    struct DDSPixelFormat
    {
        uint32_t structSize = sizeof(DDSPixelFormat);
        DDSPixelFormatFlags flags;
        char fourCC[4 ];
        uint32_t rgbBitCount;
        uint32_t redMask;
        uint32_t greenMask;
        uint32_t blueMask;
        uint32_t alphaMask;
    };

    enum class DDSHeaderFlagBits
    {
        Caps = 0x1,
        Height = 0x2,
        Width = 0x4,
        Pitch = 0x8,
        PixelFormat = 0x1000,
        MipCount = 0x20000,
        LinearSize = 0x80000,
        Depth = 0x800000
    };

    NOVA_DECLARE_FLAGS(DDSHeaderFlagBits, DDSHeaderFlags);

    enum class DDSCapsFlagBits
    {
        Complex = 0x8,
        MipMap = 0x400000,
        Texture = 0x1000
    };

    NOVA_DECLARE_FLAGS(DDSCapsFlagBits, DDSCaps);

    enum class DDCaps2FlagBits
    {
        Cubemap = 0x200,
        CubemapPositiveX = 0x400,
        CubemapNegativeX = 0x800,
        CubemapPositiveY = 0x1000,
        CubemapNegativeY = 0x2000,
        CubemapPositiveZ = 0x4000,
        CubemapNegativeZ = 0x8000,
        Volume = 0x200000
    };

    NOVA_DECLARE_FLAGS(DDCaps2FlagBits, DDSCaps2);

    enum DDS_FORMAT
    {
        DDS_FORMAT_UNKNOWN = 0,
        DDS_FORMAT_R32G32B32A32_TYPELESS = 1,
        DDS_FORMAT_R32G32B32A32_FLOAT = 2,
        DDS_FORMAT_R32G32B32A32_UINT = 3,
        DDS_FORMAT_R32G32B32A32_SINT = 4,
        DDS_FORMAT_R32G32B32_TYPELESS = 5,
        DDS_FORMAT_R32G32B32_FLOAT = 6,
        DDS_FORMAT_R32G32B32_UINT = 7,
        DDS_FORMAT_R32G32B32_SINT = 8,
        DDS_FORMAT_R16G16B16A16_TYPELESS = 9,
        DDS_FORMAT_R16G16B16A16_FLOAT = 10,
        DDS_FORMAT_R16G16B16A16_UNORM = 11,
        DDS_FORMAT_R16G16B16A16_UINT = 12,
        DDS_FORMAT_R16G16B16A16_SNORM = 13,
        DDS_FORMAT_R16G16B16A16_SINT = 14,
        DDS_FORMAT_R32G32_TYPELESS = 15,
        DDS_FORMAT_R32G32_FLOAT = 16,
        DDS_FORMAT_R32G32_UINT = 17,
        DDS_FORMAT_R32G32_SINT = 18,
        DDS_FORMAT_R32G8X24_TYPELESS = 19,
        DDS_FORMAT_D32_FLOAT_S8X24_UINT = 20,
        DDS_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
        DDS_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
        DDS_FORMAT_R10G10B10A2_TYPELESS = 23,
        DDS_FORMAT_R10G10B10A2_UNORM = 24,
        DDS_FORMAT_R10G10B10A2_UINT = 25,
        DDS_FORMAT_R11G11B10_FLOAT = 26,
        DDS_FORMAT_R8G8B8A8_TYPELESS = 27,
        DDS_FORMAT_R8G8B8A8_UNORM = 28,
        DDS_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
        DDS_FORMAT_R8G8B8A8_UINT = 30,
        DDS_FORMAT_R8G8B8A8_SNORM = 31,
        DDS_FORMAT_R8G8B8A8_SINT = 32,
        DDS_FORMAT_R16G16_TYPELESS = 33,
        DDS_FORMAT_R16G16_FLOAT = 34,
        DDS_FORMAT_R16G16_UNORM = 35,
        DDS_FORMAT_R16G16_UINT = 36,
        DDS_FORMAT_R16G16_SNORM = 37,
        DDS_FORMAT_R16G16_SINT = 38,
        DDS_FORMAT_R32_TYPELESS = 39,
        DDS_FORMAT_D32_FLOAT = 40,
        DDS_FORMAT_R32_FLOAT = 41,
        DDS_FORMAT_R32_UINT = 42,
        DDS_FORMAT_R32_SINT = 43,
        DDS_FORMAT_R24G8_TYPELESS = 44,
        DDS_FORMAT_D24_UNORM_S8_UINT = 45,
        DDS_FORMAT_R24_UNORM_X8_TYPELESS = 46,
        DDS_FORMAT_X24_TYPELESS_G8_UINT = 47,
        DDS_FORMAT_R8G8_TYPELESS = 48,
        DDS_FORMAT_R8G8_UNORM = 49,
        DDS_FORMAT_R8G8_UINT = 50,
        DDS_FORMAT_R8G8_SNORM = 51,
        DDS_FORMAT_R8G8_SINT = 52,
        DDS_FORMAT_R16_TYPELESS = 53,
        DDS_FORMAT_R16_FLOAT = 54,
        DDS_FORMAT_D16_UNORM = 55,
        DDS_FORMAT_R16_UNORM = 56,
        DDS_FORMAT_R16_UINT = 57,
        DDS_FORMAT_R16_SNORM = 58,
        DDS_FORMAT_R16_SINT = 59,
        DDS_FORMAT_R8_TYPELESS = 60,
        DDS_FORMAT_R8_UNORM = 61,
        DDS_FORMAT_R8_UINT = 62,
        DDS_FORMAT_R8_SNORM = 63,
        DDS_FORMAT_R8_SINT = 64,
        DDS_FORMAT_A8_UNORM = 65,
        DDS_FORMAT_R1_UNORM = 66,
        DDS_FORMAT_R9G9B9E5_SHAREDEXP = 67,
        DDS_FORMAT_R8G8_B8G8_UNORM = 68,
        DDS_FORMAT_G8R8_G8B8_UNORM = 69,
        DDS_FORMAT_BC1_TYPELESS = 70,
        DDS_FORMAT_BC1_UNORM = 71,
        DDS_FORMAT_BC1_UNORM_SRGB = 72,
        DDS_FORMAT_BC2_TYPELESS = 73,
        DDS_FORMAT_BC2_UNORM = 74,
        DDS_FORMAT_BC2_UNORM_SRGB = 75,
        DDS_FORMAT_BC3_TYPELESS = 76,
        DDS_FORMAT_BC3_UNORM = 77,
        DDS_FORMAT_BC3_UNORM_SRGB = 78,
        DDS_FORMAT_BC4_TYPELESS = 79,
        DDS_FORMAT_BC4_UNORM = 80,
        DDS_FORMAT_BC4_SNORM = 81,
        DDS_FORMAT_BC5_TYPELESS = 82,
        DDS_FORMAT_BC5_UNORM = 83,
        DDS_FORMAT_BC5_SNORM = 84,
        DDS_FORMAT_B5G6R5_UNORM = 85,
        DDS_FORMAT_B5G5R5A1_UNORM = 86,
        DDS_FORMAT_B8G8R8A8_UNORM = 87,
        DDS_FORMAT_B8G8R8X8_UNORM = 88,
        DDS_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
        DDS_FORMAT_B8G8R8A8_TYPELESS = 90,
        DDS_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
        DDS_FORMAT_B8G8R8X8_TYPELESS = 92,
        DDS_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
        DDS_FORMAT_BC6H_TYPELESS = 94,
        DDS_FORMAT_BC6H_UF16 = 95,
        DDS_FORMAT_BC6H_SF16 = 96,
        DDS_FORMAT_BC7_TYPELESS = 97,
        DDS_FORMAT_BC7_UNORM = 98,
        DDS_FORMAT_BC7_UNORM_SRGB = 99,
        DDS_FORMAT_AYUV = 100,
        DDS_FORMAT_Y410 = 101,
        DDS_FORMAT_Y416 = 102,
        DDS_FORMAT_NV12 = 103,
        DDS_FORMAT_P010 = 104,
        DDS_FORMAT_P016 = 105,
        DDS_FORMAT_420_OPAQUE = 106,
        DDS_FORMAT_YUY2 = 107,
        DDS_FORMAT_Y210 = 108,
        DDS_FORMAT_Y216 = 109,
        DDS_FORMAT_NV11 = 110,
        DDS_FORMAT_AI44 = 111,
        DDS_FORMAT_IA44 = 112,
        DDS_FORMAT_P8 = 113,
        DDS_FORMAT_A8P8 = 114,
        DDS_FORMAT_B4G4R4A4_UNORM = 115,
        DDS_FORMAT_P208 = 130,
        DDS_FORMAT_V208 = 131,
        DDS_FORMAT_V408 = 132,
        DDS_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
        DDS_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,
        DDS_FORMAT_FORCE_UINT = 0xffffffff
    };

    enum DDS_RESOURCE_DIMENSION
    {
        DDS_RESOURCE_DIMENSION_UNKNOWN = 0,
        DDS_RESOURCE_DIMENSION_BUFFER = 1,
        DDS_RESOURCE_DIMENSION_TEXTURE1D = 2,
        DDS_RESOURCE_DIMENSION_TEXTURE2D = 3,
        DDS_RESOURCE_DIMENSION_TEXTURE3D = 4
    };

    enum DDS_RESOURCE_MISC_FLAG
    {
        DDS_RESOURCE_MISC_GENERATE_MIPS = 0x1L,
        DDS_RESOURCE_MISC_SHARED = 0x2L,
        DDS_RESOURCE_MISC_TEXTURECUBE = 0x4L,
        DDS_RESOURCE_MISC_SHARED_KEYEDMUTEX = 0x10L,
        DDS_RESOURCE_MISC_GDI_COMPATIBLE = 0x20L
    };

    enum DDS_RESOURCE_MISC_FLAG2
    {
        DDS_ALPHA_MODE_UNKNOWN = 0x0,
        DDS_ALPHA_MODE_STRAIGHT = 0x1,
        DDS_ALPHA_MODE_PREMULTIPLIED = 0x2,
        DDS_ALPHA_MODE_OPAQUE = 0x3,
        DDS_ALPHA_MODE_CUSTOM = 0x4
    };

    struct DDSHeader
    {
        uint32_t structSize = sizeof(DDSHeader);
        DDSHeaderFlags flags;
        uint32_t height;
        uint32_t with;
        uint32_t pitchOrLinearSize;
        uint32_t depth;
        uint32_t mipCount;
        uint32_t reserved[11];
        DDSPixelFormat pixelFormat;
        DDSCaps caps;
        DDSCaps2 caps2;
        uint32_t caps3;
        uint32_t caps4;
        uint32_t reserved2;
    };

    struct DDSHeaderDXT10
    {
        DDS_FORMAT format;
        DDS_RESOURCE_DIMENSION dimension;
        DDS_RESOURCE_MISC_FLAG miscFlag;
        uint32_t arraySize;
        DDS_RESOURCE_MISC_FLAG2 miscFlags2;
    };

    
    class DDSImporter final : public TextureImporter
    {
    public:
        bool LoadFromFile(StringView filepath) override;
        bool LoadFromStream(Stream& stream) override;
        Array<String> SupportedExtensions() const override;
        Ref<Texture> CreateTexture() const override;
    private:
        DDSHeader m_Header{};
        DDSHeaderDXT10 m_HeaderDXT10{};
    };
}