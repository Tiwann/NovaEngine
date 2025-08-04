#include "VulkanExtensions.h"
#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool LoadVulkanFunctions(const VkInstance instance)
    {
        if (!instance)
            return false;

        vkSetDebugUtilsObjectName = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
        return true;
    }
}
