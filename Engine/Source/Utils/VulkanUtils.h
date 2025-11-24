#pragma once
#include <type_traits>
#include <vulkan/vulkan.h>
#include "Containers/StringView.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/VulkanExtensions.h"

namespace Nova::Vulkan
{
    template<typename T> requires std::is_pointer_v<T>
    void SetObjectName(Device* device, const VkObjectType objectType, T handle, StringView name)
    {
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        const VkDevice deviceHandle = device->GetHandle();
        VkDebugUtilsObjectNameInfoEXT info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        info.objectHandle = (uint64_t)handle;
        info.objectType = objectType;
        info.pObjectName = *name;
        if (vkSetDebugUtilsObjectName) vkSetDebugUtilsObjectName(deviceHandle, &info);
#endif
    }

    template<typename T> requires std::is_pointer_v<T>
    void SetObjectName(Nova::Device* device, const VkObjectType objectType, T handle, StringView name)
    {
        if (Device* vulkanDevice = dynamic_cast<Device*>(device))
            SetObjectName(vulkanDevice, objectType, handle, name);
    }
}

