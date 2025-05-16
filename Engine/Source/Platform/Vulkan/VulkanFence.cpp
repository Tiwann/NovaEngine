#include "VulkanFence.h"
#include "VulkanRenderer.h"
#include <vulkan/vulkan.h>


namespace Nova
{
    VulkanFence::VulkanFence(Renderer* Owner) : Fence(Owner)
    {
    }


    bool VulkanFence::Initialize(const FenceCreateInfo& CreateInfo)
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();

        VkFenceCreateInfo FenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        if (CreateInfo.Flags.Contains(FenceCreateFlagBits::Signaled))
            FenceCreateInfo.flags |= VK_FENCE_CREATE_SIGNALED_BIT;

        if (VK_FAILED(vkCreateFence(Device, &FenceCreateInfo, nullptr, &m_Handle)))
            return false;
        return true;
    }

    void VulkanFence::Destroy()
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        vkDestroyFence(Device, m_Handle, nullptr);
    }

    VkFence VulkanFence::GetHandle() const
    {
        return m_Handle;
    }

    const VkFence* VulkanFence::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
