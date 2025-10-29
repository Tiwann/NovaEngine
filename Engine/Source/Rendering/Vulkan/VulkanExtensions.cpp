#include "VulkanExtensions.h"

#define NOVA_LOAD_VK_FUNC_EXT(funcName) \
    funcName = (PFN_##funcName##EXT)vkGetInstanceProcAddr(instance, #funcName"EXT"); \
    if(!funcName) return false

namespace Nova::Vulkan
{
    bool LoadVulkanFunctions(const VkInstance instance)
    {
        if (!instance) return false;
        NOVA_LOAD_VK_FUNC_EXT(vkSetDebugUtilsObjectName);
        NOVA_LOAD_VK_FUNC_EXT(vkCreateDebugUtilsMessenger);
        return true;
    }
}
