#include "Fence.h"
#include "RenderDevice.h"
#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool Fence::Initialize(const FenceCreateInfo& createInfo)
    {
        RenderDevice* device = (RenderDevice*)createInfo.device;
        const VkDevice deviceHandle = device->GetHandle();

        VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fenceCreateInfo.flags = createInfo.flags;
        if (vkCreateFence(deviceHandle, &fenceCreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
            return false;

        m_Device = device;
        return true;
    }

    void Fence::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkDestroyFence(deviceHandle, m_Handle, nullptr);
    }

    void Fence::Wait(const uint64_t timeoutNs)
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkWaitForFences(deviceHandle, 1, &m_Handle, true, timeoutNs);
    }

    void Fence::Reset()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkResetFences(deviceHandle, 1, &m_Handle);
    }

    VkFence Fence::GetHandle() const
    {
        return m_Handle;
    }

    const VkFence* Fence::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
