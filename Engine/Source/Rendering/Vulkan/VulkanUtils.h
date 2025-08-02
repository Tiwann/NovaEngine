#pragma once
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    VkAccessFlags2 GetSourceAccessFlags(VkImageLayout layout);
    VkAccessFlags2 GetDestAccessFlags(VkImageLayout layout);
}