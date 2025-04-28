#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderer.h"

namespace Nova
{
    VulkanCommandPool::VulkanCommandPool(Renderer* Owner, const CommandPoolCreateInfo& CreateInfo)
        : CommandPool(Owner, CreateInfo)
    {
        VkCommandPoolCreateInfo PoolInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
        if (CreateInfo.Flags.Contains(CommandPoolFlagBits::Protected))
            PoolInfo.flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
        else if (CreateInfo.Flags.Contains(CommandPoolFlagBits::Transient))
            PoolInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        else if (CreateInfo.Flags.Contains(CommandPoolFlagBits::ResetCommandBuffer))
            PoolInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        const VulkanRenderer* Renderer = m_Renderer->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        PoolInfo.queueFamilyIndex = Renderer->GetGraphicsQueueFamily();

        if (VK_FAILED(vkCreateCommandPool(Device, &PoolInfo, nullptr, &m_Handle)))
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        const VulkanRenderer* Renderer = m_Renderer->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        vkDestroyCommandPool(Device, m_Handle, nullptr);
    }

    VkCommandPool VulkanCommandPool::GetHandle() const
    {
        return m_Handle;
    }

    CommandBuffer* VulkanCommandPool::AllocateCommandBuffer(const CommandBufferAllocateInfo& AllocateInfo)
    {
        VulkanCommandBuffer* Cmd = new VulkanCommandBuffer(this->As<CommandPool>(), AllocateInfo);
        m_AllocatedCommandBuffers.Add(Cmd);
        return Cmd;
    }

    void VulkanCommandPool::FreeCommandBuffer(CommandBuffer* CommandBuffer)
    {
        m_AllocatedCommandBuffers.Remove(CommandBuffer);
        delete CommandBuffer;
    }
}
