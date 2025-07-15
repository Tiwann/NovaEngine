#pragma once
#include <vulkan/vulkan.h>

#include "Runtime/Format.h"

namespace Nova::Vulkan
{
    template<typename NovaType, typename VulkanType>
    VulkanType Convert(const NovaType& value);

    template<>
    inline VkFormat Convert<Format, VkFormat>(const Format& value)
    {
        switch (value)
        {
        case Format::None:                  return VK_FORMAT_UNDEFINED;
        case Format::R8_UNORM:              return VK_FORMAT_R8_UNORM;
        case Format::R8_SNORM:              return VK_FORMAT_R8_SNORM;
        case Format::R16_USHORT:            return VK_FORMAT_R16_UINT;
        case Format::R16_SHORT:             return VK_FORMAT_R16_SINT;
        case Format::R32_FLOAT:             return VK_FORMAT_R32_SFLOAT;
        case Format::R32_UINT:              return VK_FORMAT_R32_UINT;
        case Format::R32_SINT:              return VK_FORMAT_R32_SINT;
        case Format::R8G8_UNORM:            return VK_FORMAT_R8G8_UNORM;
        case Format::R8G8_SNORM:            return VK_FORMAT_R8G8_SNORM;
        case Format::R16G16_USHORT:         return VK_FORMAT_R16G16_UINT;
        case Format::R16G16_SHORT:          return VK_FORMAT_R16G16_SINT;
        case Format::R32G32_UINT:           return VK_FORMAT_R32G32_UINT;
        case Format::R32G32_SINT:           return VK_FORMAT_R32G32_SINT;
        case Format::R32G32_FLOAT:          return VK_FORMAT_R32G32_SFLOAT;
        case Format::R8G8B8_UNORM:          return VK_FORMAT_R8G8B8_UNORM;
        case Format::R8G8B8_SNORM:          return VK_FORMAT_R8G8B8_SNORM;
        case Format::R16G16B16_USHORT:      return VK_FORMAT_R16G16B16_UINT;
        case Format::R16G16B16_SHORT:       return VK_FORMAT_R16G16B16_SINT;
        case Format::R32G32B32_UINT:        return VK_FORMAT_R32G32B32_UINT;
        case Format::R32G32B32_SINT:        return VK_FORMAT_R32G32B32_SINT;
        case Format::R32G32B32_FLOAT:       return VK_FORMAT_R32G32B32_SFLOAT;
        case Format::R8G8B8A8_UNORM:        return VK_FORMAT_R8G8B8A8_UNORM;
        case Format::R8G8B8A8_SNORM:        return VK_FORMAT_R8G8B8A8_SNORM;
        case Format::R8G8B8A8_SRGB:         return VK_FORMAT_R8G8B8A8_SRGB;
        case Format::R16G16B16A16_USHORT:   return VK_FORMAT_R16G16B16A16_UINT;
        case Format::R16G16B16A16_SHORT:    return VK_FORMAT_R16G16B16A16_SINT;
        case Format::R32G32B32A32_UINT:     return VK_FORMAT_R32G32B32A32_UINT;
        case Format::R32G32B32A32_SINT:     return VK_FORMAT_R32G32B32A32_SINT;
        case Format::R32G32B32A32_FLOAT:    return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Format::D32_FLOAT: return VK_FORMAT_D32_SFLOAT;
        case Format::D32_FLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
        default: return VK_FORMAT_UNDEFINED;
        }
    }

    template<>
    inline VkPresentModeKHR Convert<PresentMode, VkPresentModeKHR>(const PresentMode& value)
    {
        switch (value)
        {
        case PresentMode::Unknown: throw;
        case PresentMode::Immediate: return VK_PRESENT_MODE_IMMEDIATE_KHR;
        case PresentMode::Mailbox: return VK_PRESENT_MODE_MAILBOX_KHR;
        case PresentMode::Fifo: return VK_PRESENT_MODE_FIFO_KHR;
        default: throw;
        }
    }

    template<>
    inline VkFilter Convert<Filter, VkFilter>(const Filter& filter)
    {
        switch (filter)
        {
        case Filter::Nearest: return VK_FILTER_NEAREST;
        case Filter::Linear: return VK_FILTER_LINEAR;
        default: return VK_FILTER_NEAREST;
        }
    }
}
