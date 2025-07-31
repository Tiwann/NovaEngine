#include "CommandBuffer.h"
#include "Rendering/CommandBuffer.h"
#include "CommandPool.h"
#include "Buffer.h"
#include "Device.h"
#include "Conversions.h"
#include "GraphicsPipeline.h"
#include "Rendering/RenderPass.h"

#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{

    static const Rendering::RenderPass* CurrentRenderPass = nullptr;

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
        if (vkAllocateCommandBuffers(deviceHandle, &info, &m_Handle) != VK_SUCCESS)
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
        info.flags = beginInfo.flags;

        if (m_Level == Rendering::CommandBufferLevel::Secondary)
        {
            VkCommandBufferInheritanceInfo inheritanceInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
            // TODO: Write api for secondary command buffers
        }

        if (vkBeginCommandBuffer(m_Handle, &info) != VK_SUCCESS)
            return false;
        return true;
    }

    void CommandBuffer::End()
    {
        vkEndCommandBuffer(m_Handle);
    }

    void CommandBuffer::ClearColor(const Color& color, const uint32_t attachmentIndex)
    {
        VkClearAttachment clearAttachment;
        clearAttachment.colorAttachment = attachmentIndex;
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        clearAttachment.clearValue.color = VkClearColorValue{ { color.r, color.g, color.b, color.a }};

        VkClearRect clearRect;
        clearRect.rect.extent = VkExtent2D{ CurrentRenderPass->GetWidth(), CurrentRenderPass->GetHeight() };
        clearRect.rect.offset = VkOffset2D{ (int32_t)CurrentRenderPass->GetOffsetX(), (int32_t)CurrentRenderPass->GetOffsetY() };
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;
        vkCmdClearAttachments(m_Handle, 1, &clearAttachment, 1, &clearRect);
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

    void CommandBuffer::PushConstants(const ShaderStageFlags stageFlags, const size_t offset, const size_t size, const void* values, void* layout)
    {
        vkCmdPushConstants(m_Handle, (VkPipelineLayout)layout, stageFlags, offset, size, values);
    }

    void CommandBuffer::CopyBuffer(Rendering::Buffer& src, Rendering::Buffer& dest, const size_t srcOffset, const size_t destOffset, const size_t size)
    {
        VkBufferCopy2 region = { VK_STRUCTURE_TYPE_BUFFER_COPY_2 };
        region.srcOffset = srcOffset;
        region.dstOffset = destOffset;
        region.size = size;

        VkCopyBufferInfo2 copyInfo { VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2 };
        copyInfo.srcBuffer = ((Buffer&)src).GetHandle();
        copyInfo.dstBuffer = ((Buffer&)dest).GetHandle();
        copyInfo.regionCount = 1;
        copyInfo.pRegions = &region;
        vkCmdCopyBuffer2(m_Handle, &copyInfo);
    }

    void CommandBuffer::BeginRenderPass(const Rendering::RenderPass& renderPass)
    {
        Array<VkRenderingAttachmentInfo> colorAttachments;
        const uint32_t frameIndex = m_Device->GetCurrentFrameIndex();

        for (const Rendering::RenderPassAttachment& attachment : renderPass)
        {
            if (attachment.type == Rendering::AttachmentType::Depth)
                continue;

            const Texture* texture = (const Texture*)attachment.textures[frameIndex];
            const Color& clearColor = attachment.clearValue.color;

            VkRenderingAttachmentInfo colorAttachmentInfo { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
            colorAttachmentInfo.loadOp = Convert<Rendering::LoadOperation, VkAttachmentLoadOp>(attachment.loadOp);
            colorAttachmentInfo.storeOp = Convert<Rendering::StoreOperation, VkAttachmentStoreOp>(attachment.storeOp);
            colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentInfo.imageView = texture->GetImageView();
            colorAttachmentInfo.clearValue.color = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
            // TODO: Handle resolve. Do we need to resolve to swapchain directly ?
            //colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
            //colorAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            //colorAttachmentInfo.resolveImageView

            colorAttachments.Add(colorAttachmentInfo);
        }


        const Rendering::RenderPassAttachment* depthAttachment = renderPass.GetDepthAttachment();
        const Texture* texture = (const Texture*)depthAttachment->textures[frameIndex];
        const Color& clearColor = depthAttachment->clearValue.color;

        VkRenderingAttachmentInfo depthAttachmentInfo = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depthAttachmentInfo.loadOp = Convert<Rendering::LoadOperation, VkAttachmentLoadOp>(depthAttachment->loadOp);
        depthAttachmentInfo.storeOp = Convert<Rendering::StoreOperation, VkAttachmentStoreOp>(depthAttachment->storeOp);
        depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachmentInfo.imageView = texture->GetImageView();
        depthAttachmentInfo.clearValue.color = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };

        // TODO: Handle resolve. Do we need to resolve to swapchain directly ?
        //info.resolveMode
        //info.resolveImageLayout
        //info.resolveImageView

        VkRenderingInfo renderingInfo { VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderingInfo.layerCount = 1;
        renderingInfo.viewMask = 0;
        renderingInfo.renderArea.extent = { renderPass.GetWidth(), renderPass.GetHeight() };
        renderingInfo.renderArea.offset = { (int32_t)renderPass.GetOffsetX(), (int32_t)renderPass.GetOffsetY() };
        renderingInfo.colorAttachmentCount = renderPass.GetColorAttachmentCount();
        renderingInfo.pColorAttachments = colorAttachments.Data();
        renderingInfo.pDepthAttachment = renderPass.HasDepthAttachment() ? &depthAttachmentInfo : nullptr;
        vkCmdBeginRendering(m_Handle, &renderingInfo);

        CurrentRenderPass = &renderPass;
    }

    void CommandBuffer::EndRenderPass()
    {
        vkCmdEndRendering(m_Handle);
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
