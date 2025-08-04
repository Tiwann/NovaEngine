#pragma once

typedef struct VkInstance_T* VkInstance;
typedef struct VkDevice_T* VkDevice;
enum VkResult : int;

typedef VkResult(*PFN_vkSetDebugUtilsObjectNameEXT)(VkDevice, const struct VkDebugUtilsObjectNameInfoEXT*);

namespace Nova::Vulkan
{
    bool LoadVulkanFunctions(const VkInstance instance);
    inline PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectName = nullptr;
}
