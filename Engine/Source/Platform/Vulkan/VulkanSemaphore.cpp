#include "VulkanSemaphore.h"
#include "VulkanRenderer.h"
#include <vulkan/vulkan.h>

namespace Nova
{
    VulkanSemaphore::VulkanSemaphore(Renderer* Owner): Semaphore(Owner)
    {
    }

    bool VulkanSemaphore::Initialize(const SemaphoreCreateInfo& CreateInfo)
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        constexpr VkSemaphoreCreateInfo SemaphoreInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        if (VK_FAILED(vkCreateSemaphore(Device, &SemaphoreInfo, nullptr, &m_Handle)))
            return false;
        return true;
    }

    void VulkanSemaphore::Destroy()
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        vkDestroySemaphore(Device, m_Handle, nullptr);
    }

    void VulkanSemaphore::SetDebugName(const String& Name)
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();

        VkDebugUtilsObjectNameInfoEXT NameInfo { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        NameInfo.objectType = VK_OBJECT_TYPE_SEMAPHORE;
        NameInfo.objectHandle = (u64)m_Handle;
        NameInfo.pObjectName = *Name;
        Renderer->GetFunctionPointers().vkSetDebugUtilsObjectNameEXT(Device, &NameInfo);
    }

    VkSemaphore VulkanSemaphore::GetHandle() const
    {
        return m_Handle;
    }

    const VkSemaphore* VulkanSemaphore::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
