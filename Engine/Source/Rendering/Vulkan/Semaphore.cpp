#include "Semaphore.h"
#include "Device.h"
#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool Semaphore::Initialize(const SemaphoreCreateInfo& createInfo)
    {

        Device* device = static_cast<Device*>(createInfo.device);
        const VkDevice deviceHandle = device->GetHandle();
        constexpr VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        if (vkCreateSemaphore(deviceHandle, &semaphoreCreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
            return false;

        m_Device = device;
        return true;
    }

    void Semaphore::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkDestroySemaphore(deviceHandle, m_Handle, nullptr);
    }

    VkSemaphore Semaphore::GetHandle() const
    {
        return m_Handle;
    }

    const VkSemaphore* Semaphore::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
