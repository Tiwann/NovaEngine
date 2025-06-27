#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderer.h"

namespace Nova
{
    VulkanCommandPool::VulkanCommandPool(Renderer* Owner) : CommandPool(Owner)
    {

    }

    VkCommandPool VulkanCommandPool::GetHandle() const
    {
        return m_Handle;
    }

    bool VulkanCommandPool::Initialize(const CommandPoolCreateInfo& CreateInfo)
    {
        VkCommandPoolCreateInfo PoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        if (CreateInfo.Flags.Contains(CommandPoolFlagBits::Protected))
            PoolInfo.flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
        else if (CreateInfo.Flags.Contains(CommandPoolFlagBits::Transient))
            PoolInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        else if (CreateInfo.Flags.Contains(CommandPoolFlagBits::ResetCommandBuffer))
            PoolInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        PoolInfo.queueFamilyIndex = Renderer->GetGraphicsQueueFamily();

        if (VK_FAILED(vkCreateCommandPool(Device, &PoolInfo, nullptr, &m_Handle)))
            return false;
        return true;
    }

    void VulkanCommandPool::Destroy()
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        vkDestroyCommandPool(Device, m_Handle, nullptr);
    }

    void VulkanCommandPool::SetDebugName(const String& Name)
    {
        if constexpr(!RendererIsDebug)
            return;
    }
}
