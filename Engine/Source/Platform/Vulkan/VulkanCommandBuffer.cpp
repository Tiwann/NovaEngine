#include "VulkanCommandBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanRenderer.h"
#include "Rendering/CommandPool.h"

namespace Nova
{
    VulkanCommandBuffer::VulkanCommandBuffer(CommandPool* Owner, const CommandBufferAllocateInfo& AllocateInfo)
        : CommandBuffer(Owner, AllocateInfo)
    {
        VkCommandBufferAllocateInfo VkAllocInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        VkAllocInfo.commandPool = m_CommandPool->As<VulkanCommandPool>()->GetHandle();
        VkAllocInfo.commandBufferCount = 1;

        switch (m_Level)
        {
        case CommandBufferLevel::Primary:
            VkAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            break;
        case CommandBufferLevel::Secondary:
            VkAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
            break;
        }

        const VulkanRenderer* Renderer = m_CommandPool->GetOwner()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        if (VK_FAILED(vkAllocateCommandBuffers(Device, &VkAllocInfo, &m_Handle)))
        {
            throw std::runtime_error("Failed to allocate command buffers");
        }
    }

    VulkanCommandBuffer::~VulkanCommandBuffer()
    {
        const VulkanRenderer* Renderer = m_CommandPool->GetOwner()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        vkFreeCommandBuffers(Device, m_CommandPool->As<VulkanCommandPool>()->GetHandle(), 1, &m_Handle);
    }

    VkCommandBuffer VulkanCommandBuffer::GetHandle() const
    {
        return m_Handle;
    }

    const VkCommandBuffer* VulkanCommandBuffer::GetHandlePtr() const
    {
        return &m_Handle;
    }

    bool VulkanCommandBuffer::IsValid() const
    {
        return m_Handle;
    }

    bool VulkanCommandBuffer::Begin(const CommandBufferBeginInfo& BeginInfo)
    {
        VkCommandBufferBeginInfo VkInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        if (BeginInfo.Flags.Contains(CommandBufferUsageFlagBits::OneTimeSubmit))
            VkInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        else if (BeginInfo.Flags.Contains(CommandBufferUsageFlagBits::Simultaneous))
            VkInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        else if (BeginInfo.Flags.Contains(CommandBufferUsageFlagBits::RenderPassContinue))
            VkInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

        if (m_Level == CommandBufferLevel::Secondary)
        {
            VkCommandBufferInheritanceInfo InheritanceInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
            VkInfo.pInheritanceInfo = &InheritanceInfo;
        }

        if (VK_FAILED(vkBeginCommandBuffer(m_Handle, &VkInfo)))
            return false;
        return true;
    }

    bool VulkanCommandBuffer::End()
    {
        if (VK_FAILED(vkEndCommandBuffer(m_Handle)))
            return false;
        return true;
    }
}
