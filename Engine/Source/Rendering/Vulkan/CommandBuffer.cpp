#include "CommandBuffer.h"
#include "Rendering/CommandBuffer.h"
#include "CommandPool.h"
#include "Buffer.h"
#include "Device.h"
#include "Conversions.h"

#include <vulkan/vulkan.h>

#include "GraphicsPipeline.h"


namespace Nova::Vulkan
{

    bool CommandBuffer::Allocate(const Rendering::CommandBufferAllocateInfo& allocateInfo)
    {
        Device* device = static_cast<Device*>(allocateInfo.device);
        m_CommandPool = allocateInfo.commandPool;
        m_Level = allocateInfo.level;
        m_Device = device;

        CommandPool* pool = static_cast<CommandPool*>(m_CommandPool);

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
        CommandPool* pool = static_cast<CommandPool*>(m_CommandPool);
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

    void CommandBuffer::ClearDepth(float depth, uint32_t stencil)
    {
    }

    void CommandBuffer::BindGraphicsPipeline(const Rendering::GraphicsPipeline& pipeline)
    {
        vkCmdBindPipeline(m_Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, ((const GraphicsPipeline&)pipeline).GetHandle());
    }

    void CommandBuffer::BindComputePipeline(const Rendering::ComputePipeline& pipeline)
    {
        vkCmdBindPipeline(m_Handle, VK_PIPELINE_BIND_POINT_COMPUTE, ((const GraphicsPipeline&)pipeline).GetHandle());
    }

    void CommandBuffer::BindVertexBuffer(const Rendering::Buffer& vertexBuffer, const size_t offset)
    {
        const Buffer& vertexBuff = (const Buffer&)vertexBuffer;
        const VkCommandBuffer cmdBuff = GetHandle();
        vkCmdBindVertexBuffers(cmdBuff, 0, 1, vertexBuff.GetHandlePtr(), &offset);
    }

    void CommandBuffer::BindIndexBuffer(const Rendering::Buffer& indexBuffer, const size_t offset, const Format indexFormat)
    {
        const Buffer& indexBuff = (const Buffer&)indexBuffer;
        const VkCommandBuffer cmdBuff = GetHandle();
        vkCmdBindIndexBuffer(cmdBuff, indexBuff.GetHandle(), offset, Convert<Format, VkIndexType>(indexFormat));
    }

    void CommandBuffer::SetViewport(const float x, const float y, const float width, const float height, const float minDepth, const float maxDepth)
    {
        VkViewport viewport { };
        viewport.x = x;
        viewport.y = y + height;
        viewport.width = width;
        viewport.height = -height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;
        vkCmdSetViewport(m_Handle, 0, 1, &viewport);
    }

    void CommandBuffer::SetScissor(const int32_t x, const int32_t y, const int32_t width, const int32_t height)
    {
        VkRect2D rect { };
        rect.offset.x = x;
        rect.offset.y = y;
        rect.extent.width = width;
        rect.extent.height = height;
        vkCmdSetScissor(m_Handle, 0, 1, &rect);
    }

    void CommandBuffer::DrawIndexed(const size_t count, const size_t offset)
    {
        vkCmdDrawIndexed(m_Handle, count, 1, 0, offset, 0);
    }

    void CommandBuffer::Dispatch(const uint32_t groupCountX, const uint32_t groupCountY, const uint32_t groupCountZ)
    {
        vkCmdDispatch(m_Handle, groupCountX, groupCountY, groupCountZ);
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
