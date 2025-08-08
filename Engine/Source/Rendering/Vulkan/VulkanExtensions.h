#pragma once
#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool LoadVulkanFunctions(const VkInstance instance);
    inline PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectName = nullptr;
}
