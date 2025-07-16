#pragma once
#include <cstddef>

namespace Nova
{
    enum class Format
    {
        None,
        R8_UNORM,
        R8_SNORM,
        R16_USHORT,
        R16_SHORT,
        R32_FLOAT,
        R32_UINT,
        R32_SINT,
        R8G8_UNORM,
        R8G8_SNORM,
        R16G16_USHORT,
        R16G16_SHORT,
        R32G32_UINT,
        R32G32_SINT,
        R32G32_FLOAT,
        R8G8B8_UNORM,
        R8G8B8_SNORM,
        R16G16B16_USHORT,
        R16G16B16_SHORT,
        R32G32B32_UINT,
        R32G32B32_SINT,
        R32G32B32_FLOAT,
        R8G8B8A8_UNORM,
        R8G8B8A8_SNORM,
        R8G8B8A8_SRGB,
        R16G16B16A16_USHORT,
        R16G16B16A16_SHORT,
        R32G32B32A32_UINT,
        R32G32B32A32_SINT,
        R32G32B32A32_FLOAT,
        D32_FLOAT,
        D32_FLOAT_S8_UINT,
        D24_UNORM_S8_UINT,
        ASTC_4X4_UNORM,
        ASTC_6X6_UNORM,
        ASTC_8X8_UNORM,
        ASTC_12X12_UNORM,
        ASTC_4X4_SRGB,
        ASTC_6X6_SRGB,
        ASTC_8X8_SRGB,
        ASTC_12X12_SRGB,

        Float = R32_FLOAT,
        Vector2 = R32G32_FLOAT,
        Vector3 = R32G32B32_FLOAT,
        Vector4 = R32G32B32A32_FLOAT,
    };

    static size_t GetFormatSize(const Format Format)
    {
        switch (Format)
        {
        case Format::None:                 return 0ULL;
        case Format::R8G8B8A8_UNORM:       return 4ULL;
        case Format::R16G16B16A16_USHORT:  return 4ULL * 2ULL;
        case Format::R32G32B32A32_FLOAT:   return 4ULL * 4ULL;
        case Format::R8_UNORM:             return 1ULL;
        case Format::R8_SNORM:             return 1ULL * 1ULL;
        case Format::R16_USHORT:           return 2ULL * 1ULL;
        case Format::R16_SHORT:            return 2ULL * 1ULL;
        case Format::R32_FLOAT:            return 4ULL * 1ULL;
        case Format::R32_UINT:             return 4ULL * 1ULL;
        case Format::R32_SINT:             return 4ULL * 1ULL;
        case Format::R8G8_UNORM:           return 2ULL * 1ULL;
        case Format::R8G8_SNORM:           return 2ULL * 1ULL;
        case Format::R16G16_USHORT:        return 2ULL * 2ULL;
        case Format::R16G16_SHORT:         return 2ULL * 2ULL;
        case Format::R32G32_UINT:          return 2ULL * 4ULL;
        case Format::R32G32_SINT:          return 2ULL * 4ULL;
        case Format::R32G32_FLOAT:         return 2ULL * 4ULL;
        case Format::R8G8B8_UNORM:         return 3ULL * 1ULL;
        case Format::R8G8B8_SNORM:         return 3ULL * 1ULL;
        case Format::R16G16B16_USHORT:     return 3ULL * 2ULL;
        case Format::R16G16B16_SHORT:      return 3ULL * 2ULL;
        case Format::R32G32B32_UINT:       return 3ULL * 4ULL;
        case Format::R32G32B32_SINT:       return 3ULL * 4ULL;
        case Format::R32G32B32_FLOAT:      return 3ULL * 4ULL;
        case Format::R8G8B8A8_SNORM:       return 4ULL * 1ULL;
        case Format::R16G16B16A16_SHORT:   return 4ULL * 2ULL;
        case Format::R32G32B32A32_UINT:    return 4ULL * 4ULL;
        case Format::R32G32B32A32_SINT:    return 4ULL * 4ULL;
        case Format::R8G8B8A8_SRGB:        return 4ULL * 1ULL;
        case Format::D32_FLOAT:            return 4ULL;
        case Format::D32_FLOAT_S8_UINT:    return 5ULL;
        case Format::D24_UNORM_S8_UINT:     return 4ULL;
        default:                           return 0ULL;
        }
    }

    static size_t GetFormatBytesPerChannel(const Format Format)
    {
        switch (Format)
        {
        case Format::None:  return 0;
        case Format::R8_UNORM: return 1ULL;
        case Format::R8_SNORM: return 1ULL;
        case Format::R16_USHORT: return 2ULL;
        case Format::R16_SHORT: return 2ULL;
        case Format::R32_FLOAT: return 4ULL;
        case Format::R32_UINT: return 4ULL;
        case Format::R32_SINT: return 4ULL;
        case Format::R8G8_UNORM: return 1ULL;
        case Format::R8G8_SNORM: return 1ULL;
        case Format::R16G16_USHORT: return 2ULL;
        case Format::R16G16_SHORT: return 2ULL;
        case Format::R32G32_UINT: return 4ULL;
        case Format::R32G32_SINT: return 4ULL;
        case Format::R32G32_FLOAT: return 4ULL;
        case Format::R8G8B8_UNORM: return 1ULL;
        case Format::R8G8B8_SNORM: return 1ULL;
        case Format::R16G16B16_USHORT: return 2ULL;
        case Format::R16G16B16_SHORT: return 2ULL;
        case Format::R32G32B32_UINT: return 4ULL;
        case Format::R32G32B32_SINT: return 4ULL;
        case Format::R32G32B32_FLOAT: return 4ULL;
        case Format::R8G8B8A8_UNORM: return 1ULL;
        case Format::R8G8B8A8_SNORM: return 1ULL;
        case Format::R16G16B16A16_USHORT: return 2ULL;
        case Format::R16G16B16A16_SHORT: return 2ULL;
        case Format::R32G32B32A32_UINT: return 4ULL;
        case Format::R32G32B32A32_SINT: return 4ULL;
        case Format::R32G32B32A32_FLOAT: return 4ULL;
        case Format::R8G8B8A8_SRGB: return 1ULL;
        case Format::D32_FLOAT: return 4ULL;
        case Format::D32_FLOAT_S8_UINT: return 4ULL;
        case Format::D24_UNORM_S8_UINT: return 3ULL;
        default: return 0;
        }
    }

    static size_t GetFormatComponentCount(const Format Format)
    {
        switch (Format)
        {
        case Format::None: return 0ULL;
        case Format::R8_UNORM: return 1ULL;
        case Format::R8_SNORM: return 1ULL;
        case Format::R16_USHORT: return 1ULL;
        case Format::R16_SHORT: return 1ULL;
        case Format::R32_FLOAT: return 1ULL;
        case Format::R32_UINT: return 1ULL;
        case Format::R32_SINT: return 1ULL;
        case Format::R8G8_UNORM: return 2ULL;
        case Format::R8G8_SNORM: return 2ULL;
        case Format::R16G16_USHORT: return 2ULL;
        case Format::R16G16_SHORT: return 2ULL;
        case Format::R32G32_UINT: return 2ULL;
        case Format::R32G32_SINT: return 2ULL;
        case Format::R32G32_FLOAT: return 2ULL;
        case Format::R8G8B8_UNORM: return 3ULL;
        case Format::R8G8B8_SNORM: return 3ULL;
        case Format::R16G16B16_USHORT: return 3ULL;
        case Format::R16G16B16_SHORT: return 3ULL;
        case Format::R32G32B32_UINT: return 3ULL;
        case Format::R32G32B32_SINT: return 3ULL;
        case Format::R32G32B32_FLOAT: return 3ULL;
        case Format::R8G8B8A8_UNORM: return 4ULL;
        case Format::R8G8B8A8_SNORM: return 4ULL;
        case Format::R16G16B16A16_USHORT: return 4ULL;
        case Format::R16G16B16A16_SHORT: return 4ULL;
        case Format::R32G32B32A32_UINT: return 4ULL;
        case Format::R32G32B32A32_SINT: return 4ULL;
        case Format::R32G32B32A32_FLOAT: return 4ULL;
        case Format::R8G8B8A8_SRGB: return 4ULL;
        case Format::D32_FLOAT: return 1ULL;
        case Format::D32_FLOAT_S8_UINT: return 2ULL;
        case Format::D24_UNORM_S8_UINT: return 2ULL;
        default: throw;
        }
    }
}