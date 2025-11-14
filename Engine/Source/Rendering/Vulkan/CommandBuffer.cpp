#include "CommandBuffer.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/BlitRegion.h"
#include "CommandPool.h"
#include "Buffer.h"
#include "Device.h"
#include "Conversions.h"
#include "GraphicsPipeline.h"
#include "Rendering/RenderPass.h"
#include "VulkanExtensions.h"
#include "VulkanUtils.h"
#include <vulkan/vulkan.h>

#include "Material.h"
#include "Shader.h"

namespace Nova::Vulkan
{

    static const Rendering::RenderPass* s_CurrentRenderPass = nullptr;

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

    void CommandBuffer::SetName(const StringView name)
    {
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        const VkDevice deviceHandle = m_Device->GetHandle();
        VkDebugUtilsObjectNameInfoEXT info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        info.objectHandle = (uint64_t)m_Handle;
        info.objectType = VK_OBJECT_TYPE_COMMAND_BUFFER;
        info.pObjectName = *name;
        vkSetDebugUtilsObjectName(deviceHandle, &info);
#endif
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

    void CommandBuffer::ExecuteCommandBuffers(const Array<Rendering::CommandBuffer*>& commandBuffers)
    {
        const auto toHandles = [](const Rendering::CommandBuffer* commandBuffer) -> VkCommandBuffer
        {
            const CommandBuffer* cmdBuff = (const CommandBuffer*)commandBuffer;
            return cmdBuff->GetHandle();
        };

        Array<VkCommandBuffer> cmdBuffs = commandBuffers.Transform<VkCommandBuffer>(toHandles);

        vkCmdExecuteCommands(m_Handle, cmdBuffs.Count(), cmdBuffs.Data());
    }

    void CommandBuffer::ClearColor(const Color& color, const uint32_t attachmentIndex)
    {
        VkClearAttachment clearAttachment;
        clearAttachment.colorAttachment = attachmentIndex;
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        clearAttachment.clearValue.color = VkClearColorValue{ { color.r, color.g, color.b, color.a }};

        VkClearRect clearRect;
        clearRect.rect.extent = VkExtent2D{ s_CurrentRenderPass->GetWidth(), s_CurrentRenderPass->GetHeight() };
        clearRect.rect.offset = VkOffset2D{ (int32_t)s_CurrentRenderPass->GetOffsetX(), (int32_t)s_CurrentRenderPass->GetOffsetY() };
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;
        vkCmdClearAttachments(m_Handle, 1, &clearAttachment, 1, &clearRect);
    }

    void CommandBuffer::ClearDepthStencil(const float depth, const uint32_t stencil, const uint32_t attachmentIndex)
    {
        VkClearAttachment clearAttachment;
        clearAttachment.colorAttachment = attachmentIndex;
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        clearAttachment.clearValue.depthStencil = { depth, stencil };

        VkClearRect clearRect;
        clearRect.rect.extent = VkExtent2D{ s_CurrentRenderPass->GetWidth(), s_CurrentRenderPass->GetHeight() };
        clearRect.rect.offset = VkOffset2D{ (int32_t)s_CurrentRenderPass->GetOffsetX(), (int32_t)s_CurrentRenderPass->GetOffsetY() };
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;
        vkCmdClearAttachments(m_Handle, 1, &clearAttachment, 1, &clearRect);
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

    void CommandBuffer::BindShaderBindingSet(const Rendering::Shader& shader, const Rendering::ShaderBindingSet& bindingSet)
    {
        const Shader& sh = (const Shader&)shader;

        VkBindDescriptorSetsInfo info = { VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO };
        info.layout = sh.GetPipelineLayout();
        info.firstSet = 0;
        info.descriptorSetCount = 1;
        info.pDescriptorSets = ((const ShaderBindingSet&)bindingSet).GetHandlePtr();
        info.stageFlags = Convert<ShaderStageFlags, VkShaderStageFlags>(sh.GetShaderStageFlags());
        info.dynamicOffsetCount = 0;
        info.pDynamicOffsets = nullptr;
        vkCmdBindDescriptorSets2(m_Handle, &info);
    }

    void CommandBuffer::BindMaterial(const Rendering::Material& material)
    {
        Ref<Shader> shader { material.GetShader() };
        if (!shader) return;

        const Array<Ref<Rendering::ShaderBindingSet>>& bindingSets = material.GetBindingSets();
        for (auto bindingSet : bindingSets)
            BindShaderBindingSet(*shader, *bindingSet);
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

    void CommandBuffer::Draw(const size_t vertexCount, const size_t instanceCount, const size_t firstIndex, const size_t firstInstance)
    {
        vkCmdDraw(m_Handle, vertexCount, instanceCount, firstIndex, firstInstance);
    }

    void CommandBuffer::Dispatch(const uint32_t groupCountX, const uint32_t groupCountY, const uint32_t groupCountZ)
    {
        vkCmdDispatch(m_Handle, groupCountX, groupCountY, groupCountZ);
    }

    void CommandBuffer::DispatchIndirect(const Rendering::Buffer& buffer, const size_t offset)
    {
        vkCmdDispatchIndirect(m_Handle, ((const Buffer&)buffer).GetHandle(), offset);
    }

    void CommandBuffer::PushConstants(const Rendering::Shader& shader, const ShaderStageFlags stageFlags, const size_t offset, const size_t size, const void* values)
    {
        vkCmdPushConstants(m_Handle, ((const Shader&)shader).GetPipelineLayout(), Convert<ShaderStageFlags, VkShaderStageFlags>(stageFlags), offset, size, values);
    }

    void CommandBuffer::UpdateBuffer(const Rendering::Buffer& buffer, const size_t offset, const size_t size, const void* data)
    {
        vkCmdUpdateBuffer(m_Handle, ((const Buffer&)buffer).GetHandle(), offset, size, data);
    }

    void CommandBuffer::BufferCopy(const Rendering::Buffer& src, const Rendering::Buffer& dest, const size_t srcOffset, const size_t destOffset, const size_t size)
    {
        VkBufferCopy2 region = { VK_STRUCTURE_TYPE_BUFFER_COPY_2 };
        region.srcOffset = srcOffset;
        region.dstOffset = destOffset;
        region.size = size;

        VkCopyBufferInfo2 copyInfo { VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2 };
        copyInfo.srcBuffer = ((const Buffer&)src).GetHandle();
        copyInfo.dstBuffer = ((const Buffer&)dest).GetHandle();
        copyInfo.regionCount = 1;
        copyInfo.pRegions = &region;
        vkCmdCopyBuffer2(m_Handle, &copyInfo);
    }

    void CommandBuffer::Blit(const Rendering::Texture& src, const Rendering::BlitRegion& srcRegion, const Rendering::Texture& dest, const Rendering::BlitRegion& destRegion, const Filter filter)
    {
        const Texture& source = (Texture&)src;
        const Texture& destination = (Texture&)dest;
        //NOVA_ASSERT(source.GetFormat() != destination.GetFormat(), "Source and destination textures must have the same format");

        const VkImageLayout sourceInitialLayout = (VkImageLayout)source.GetImageLayout();
        const VkImageLayout destInitialLayout = (VkImageLayout)destination.GetImageLayout();

        VkImageMemoryBarrier2 inBarriers[2] = {  };
        inBarriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        inBarriers[0].oldLayout = sourceInitialLayout;
        inBarriers[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        inBarriers[0].srcAccessMask = GetSourceAccessFlags(sourceInitialLayout);
        inBarriers[0].dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
        inBarriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        inBarriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        inBarriers[0].image = source.GetImage();
        inBarriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        inBarriers[0].subresourceRange.baseMipLevel = 0;
        inBarriers[0].subresourceRange.levelCount = 1;
        inBarriers[0].subresourceRange.baseArrayLayer = 0;
        inBarriers[0].subresourceRange.layerCount = 1;

        inBarriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        inBarriers[1].oldLayout = destInitialLayout;
        inBarriers[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        inBarriers[1].srcAccessMask = GetDestAccessFlags(destInitialLayout);
        inBarriers[1].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        inBarriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        inBarriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        inBarriers[1].image = destination.GetImage();
        inBarriers[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        inBarriers[1].subresourceRange.baseMipLevel = 0;
        inBarriers[1].subresourceRange.levelCount = 1;
        inBarriers[1].subresourceRange.baseArrayLayer = 0;
        inBarriers[1].subresourceRange.layerCount = 1;

        VkDependencyInfo inDependency = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        inDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        inDependency.imageMemoryBarrierCount = std::size(inBarriers);
        inDependency.pImageMemoryBarriers = inBarriers;
        vkCmdPipelineBarrier2(m_Handle, &inDependency);

        VkImageBlit2 region = { VK_STRUCTURE_TYPE_IMAGE_BLIT_2 };
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.layerCount = 1;
        region.srcSubresource.baseArrayLayer = 0;
        region.srcSubresource.mipLevel = srcRegion.mipLevel;

        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.layerCount = 1;
        region.dstSubresource.baseArrayLayer = 0;
        region.dstSubresource.mipLevel = destRegion.mipLevel;

        region.srcOffsets[0] = VkOffset3D { (int32_t)srcRegion.x, (int32_t)srcRegion.y, 0 };
        region.srcOffsets[1] = VkOffset3D { (int32_t)srcRegion.width, (int32_t)srcRegion.height, 1 };
        region.dstOffsets[0] = VkOffset3D { (int32_t)destRegion.x, (int32_t)destRegion.y, 0 };
        region.dstOffsets[1] = VkOffset3D { (int32_t)destRegion.width, (int32_t)destRegion.height, 1 };

        VkBlitImageInfo2 blitInfo = { VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2 };
        blitInfo.filter = Convert<Filter, VkFilter>(filter);
        blitInfo.srcImage = source.GetImage();
        blitInfo.dstImage = destination.GetImage();
        blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        blitInfo.regionCount = 1;
        blitInfo.pRegions = &region;
        vkCmdBlitImage2(m_Handle, &blitInfo);

        VkImageMemoryBarrier2 outBarriers[2] = {  };
        outBarriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        outBarriers[0].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        outBarriers[0].newLayout = sourceInitialLayout;
        outBarriers[0].srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
        outBarriers[0].dstAccessMask = GetDestAccessFlags(sourceInitialLayout);
        outBarriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        outBarriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        outBarriers[0].image = source.GetImage();
        outBarriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        outBarriers[0].subresourceRange.baseMipLevel = 0;
        outBarriers[0].subresourceRange.levelCount = 1;
        outBarriers[0].subresourceRange.baseArrayLayer = 0;
        outBarriers[0].subresourceRange.layerCount = 1;

        outBarriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        outBarriers[1].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        outBarriers[1].newLayout = destInitialLayout == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_GENERAL : destInitialLayout;
        outBarriers[1].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        outBarriers[1].dstAccessMask = GetDestAccessFlags(destInitialLayout);
        outBarriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        outBarriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        outBarriers[1].image = destination.GetImage();
        outBarriers[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        outBarriers[1].subresourceRange.baseMipLevel = 0;
        outBarriers[1].subresourceRange.levelCount = 1;
        outBarriers[1].subresourceRange.baseArrayLayer = 0;
        outBarriers[1].subresourceRange.layerCount = 1;

        VkDependencyInfo outDependency = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        outDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        outDependency.imageMemoryBarrierCount = std::size(outBarriers);
        outDependency.pImageMemoryBarriers = outBarriers;
        vkCmdPipelineBarrier2(m_Handle, &outDependency);
    }

    void CommandBuffer::Blit(const Rendering::Texture& src, const Rendering::Texture& dest, const Filter filter)
    {
        const Rendering::BlitRegion srcRegion = { 0, 0, src.GetWidth(), src.GetHeight(), 0 };
        const Rendering::BlitRegion destRegion = { 0, 0, dest.GetWidth(), dest.GetHeight(), 0 };
        Blit(src, srcRegion, dest, destRegion, filter);
    }

    void CommandBuffer::BeginRenderPass(const Rendering::RenderPass& renderPass)
    {
        Array<VkRenderingAttachmentInfo> colorAttachments;

        for (const Rendering::RenderPassAttachment& attachment : renderPass)
        {
            if (attachment.type == Rendering::AttachmentType::Depth)
                continue;

            const Color& clearColor = attachment.clearValue.color;

            VkRenderingAttachmentInfo colorAttachmentInfo { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
            colorAttachmentInfo.loadOp = Convert<Rendering::LoadOperation, VkAttachmentLoadOp>(attachment.loadOp);
            colorAttachmentInfo.storeOp = Convert<Rendering::StoreOperation, VkAttachmentStoreOp>(attachment.storeOp);
            colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentInfo.imageView = ((Texture*)attachment.texture)->GetImageView();
            colorAttachmentInfo.clearValue.color = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
            if (attachment.resolveTexture)
            {
                colorAttachmentInfo.resolveMode = Convert<ResolveMode, VkResolveModeFlagBits>(attachment.resolveMode);
                colorAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                colorAttachmentInfo.resolveImageView = ((Texture*)attachment.resolveTexture)->GetImageView();
            }
            colorAttachments.Add(colorAttachmentInfo);
        }


        const Rendering::RenderPassAttachment* depthAttachment = renderPass.GetDepthAttachment();
        VkRenderingAttachmentInfo depthAttachmentInfo = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        if (renderPass.HasDepthAttachment())
        {
            depthAttachmentInfo.loadOp = Convert<Rendering::LoadOperation, VkAttachmentLoadOp>(depthAttachment->loadOp);
            depthAttachmentInfo.storeOp = Convert<Rendering::StoreOperation, VkAttachmentStoreOp>(depthAttachment->storeOp);
            depthAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthAttachmentInfo.imageView = ((Texture*)depthAttachment->texture)->GetImageView();
            depthAttachmentInfo.clearValue.depthStencil = { depthAttachment->clearValue.depth, depthAttachment->clearValue.stencil};
            if (depthAttachment->resolveTexture)
            {
                depthAttachmentInfo.resolveMode = Convert<ResolveMode, VkResolveModeFlagBits>(depthAttachment->resolveMode);
                depthAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
                depthAttachmentInfo.resolveImageView = ((Texture*)depthAttachment->resolveTexture)->GetImageView();
            }
        }

        VkRenderingInfo renderingInfo { VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderingInfo.layerCount = 1;
        renderingInfo.viewMask = 0;
        renderingInfo.renderArea.extent = { renderPass.GetWidth(), renderPass.GetHeight() };
        renderingInfo.renderArea.offset = { (int32_t)renderPass.GetOffsetX(), (int32_t)renderPass.GetOffsetY() };
        renderingInfo.colorAttachmentCount = renderPass.GetColorAttachmentCount();
        renderingInfo.pColorAttachments = colorAttachments.Data();
        renderingInfo.pDepthAttachment = renderPass.HasDepthAttachment() ? &depthAttachmentInfo : nullptr;
        vkCmdBeginRendering(m_Handle, &renderingInfo);

        s_CurrentRenderPass = &renderPass;
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
