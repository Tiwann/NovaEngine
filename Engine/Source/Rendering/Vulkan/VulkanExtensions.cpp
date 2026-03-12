#include "VulkanExtensions.h"

#define NOVA_LOAD_VK_FUNC_EXT(funcName) \
    funcName = (PFN_##funcName##EXT)vkGetInstanceProcAddr(instance, #funcName"EXT"); \
    if(!funcName) return false

#define NOVA_LOAD_VK_FUNC_KHR(funcName) \
funcName = (PFN_##funcName##KHR)vkGetInstanceProcAddr(instance, #funcName"KHR"); \
if(!funcName) return false

namespace Nova::Vulkan
{
    bool LoadVulkanFunctions(const VkInstance instance)
    {
        if (!instance) return false;
        NOVA_LOAD_VK_FUNC_EXT(vkSetDebugUtilsObjectName);
        NOVA_LOAD_VK_FUNC_EXT(vkCreateDebugUtilsMessenger);
        NOVA_LOAD_VK_FUNC_EXT(vkDestroyDebugUtilsMessenger);
        NOVA_LOAD_VK_FUNC_KHR(vkGetSwapchainStatus);
        return true;
    }
}
