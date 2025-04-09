#pragma once

namespace Nova
{
    enum class Format
    {
        NONE,
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
        R16G16B16A16_USHORT,
        R16G16B16A16_SHORT,
        R32G32B32A32_UINT,
        R32G32B32A32_SINT,
        R32G32B32A32_FLOAT
    };

    static size_t GetBytesPerPixel(const Format Format)
    {
        switch (Format)
        {
        case Format::NONE:                 return 0ULL;
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
        default:                           return 0ULL;
        }
    }

    static size_t GetBytesPerChannel(const Format Format)
    {
        switch (Format)
        {
        case Format::NONE:  return 0;
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
        default: return 0;
        }
    }
}