#include "CommandBuffer.h"
#include "Rendering/CommandBuffer.h"
#include "CommandPool.h"
#include <vulkan/vulkan.h>

#include "Device.h"


namespace Nova::Vulkan
{

    bool CommandBuffer::Allocate(const Rendering::CommandBufferAllocateInfo& allocateInfo)
    {
        Device* device = dynamic_cast<Device*>(allocateInfo.device);
        m_CommandPool = allocateInfo.commandPool;
        m_Level = allocateInfo.level;
        m_Device = device;

        CommandPool* pool = dynamic_cast<CommandPool*>(m_CommandPool);

        VkCommandBufferAllocateInfo info { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        info.commandPool = pool->GetHandle();
        info.commandBufferCount = 1;

        switch (m_Level)
        {
        case Rendering::CommandBufferLevel::Primary:
            info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            break;
        case Rendering::CommandBufferLevel::Secondary:
            info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
            break;
        }


        const VkDevice deviceHandle = device->GetHandle();
        if (vkAllocateCommandBuffers(deviceHandle, &info, &m_Handle))
            return false;
        return true;
    }

    void CommandBuffer::Free()
    {
        CommandPool* pool = dynamic_cast<CommandPool*>(m_CommandPool);
        const VkDevice deviceHandle = m_Device->GetHandle();
        const VkCommandPool commandPoolHandle = pool->GetHandle();
        vkFreeCommandBuffers(deviceHandle, commandPoolHandle, 1, &m_Handle);
    }

    bool CommandBuffer::Begin(const Rendering::CommandBufferBeginInfo& beginInfo)
    {
        VkCommandBufferBeginInfo info { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        info.flags = beginInfo.Flags;

        if (m_Level == Rendering::CommandBufferLevel::Secondary)
        {
            VkCommandBufferInheritanceInfo inheritanceInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
            // TODO: Write api for secondary command buffers
        }

        if (vkBeginCommandBuffer(m_Handle, &info))
            return false;
        return true;
    }

    void CommandBuffer::End()
    {
        vkEndCommandBuffer(m_Handle);
    }

    void CommandBuffer::ClearColor(const Color& color)
    {

    }

    void CommandBuffer::ClearDepth(float depth, uint8_t stencil)
    {
    }

    VkCommandBuffer CommandBuffer::GetHandle() const
    {
        return m_Handle;
    }

    const VkCommandBuffer* CommandBuffer::GetHandlePtr() const
    {
        return &m_Handle;
    }


}
