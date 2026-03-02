#include "CommandBuffer.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/BlitRegion.h"
#include "CommandPool.h"
#include "Buffer.h"
#include "RenderDevice.h"
#include "Conversions.h"
#include "GraphicsPipeline.h"
#include "Rendering/RenderPass.h"
#include "VulkanUtils.h"
#include "Material.h"
#include "Shader.h"
#include "Rendering/ResourceBarrier.h"
#include "Utils/VulkanUtils.h"

#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    static RenderPassBeginInfo* s_RenderPass = nullptr;

    bool CommandBuffer::Allocate(const CommandBufferAllocateInfo& allocateInfo)
    {
        RenderDevice* device = static_cast<RenderDevice*>(allocateInfo.device);
        m_CommandPool = allocateInfo.commandPool;
        m_Level = allocateInfo.level;
        m_Device = device;

        CommandPool* pool = static_cast<CommandPool*>(m_CommandPool);

        VkCommandBufferAllocateInfo info { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
        info.commandPool = pool->GetHandle();
        info.commandBufferCount = 1;

        switch (m_Level)
        {
        case CommandBufferLevel::Primary:
            info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            break;
        case CommandBufferLevel::Secondary:
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
        SetVkObjectDebugName(m_Device, VK_OBJECT_TYPE_COMMAND_BUFFER, m_Handle, name);
    }

    bool CommandBuffer::Begin(const CommandBufferBeginInfo& beginInfo)
    {
        VkCommandBufferBeginInfo info { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        info.flags = beginInfo.flags;

        if (m_Level == CommandBufferLevel::Secondary)
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

    void CommandBuffer::ExecuteCommandBuffers(const Array<Nova::CommandBuffer*>& commandBuffers)
    {
        const auto toHandles = [](const Nova::CommandBuffer* commandBuffer) -> VkCommandBuffer
        {
            return ((const CommandBuffer*)commandBuffer)->GetHandle();
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
        clearRect.rect.extent = VkExtent2D{ s_RenderPass->renderArea.width, s_RenderPass->renderArea.height };
        clearRect.rect.offset = VkOffset2D{ static_cast<int32_t>(s_RenderPass->renderArea.x), static_cast<int32_t>(s_RenderPass->renderArea.y) };
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;
        vkCmdClearAttachments(m_Handle, 1, &clearAttachment, 1, &clearRect);
    }

    void CommandBuffer::ClearDepthStencil(const float depth, const uint32_t stencil)
    {
        VkClearAttachment clearAttachment;
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        clearAttachment.clearValue.depthStencil = { depth, stencil };

        VkClearRect clearRect;
        clearRect.rect.extent = VkExtent2D{ s_RenderPass->renderArea.width, s_RenderPass->renderArea.height };
        clearRect.rect.offset = VkOffset2D{ static_cast<int32_t>(s_RenderPass->renderArea.x), static_cast<int32_t>(s_RenderPass->renderArea.y) };
        clearRect.baseArrayLayer = 0;
        clearRect.layerCount = 1;
        vkCmdClearAttachments(m_Handle, 1, &clearAttachment, 1, &clearRect);
    }

    void CommandBuffer::BindGraphicsPipeline(const Nova::GraphicsPipeline& pipeline)
    {
        vkCmdBindPipeline(m_Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, ((const GraphicsPipeline&)pipeline).GetHandle());
    }

    void CommandBuffer::BindComputePipeline(const Nova::ComputePipeline& pipeline)
    {
        vkCmdBindPipeline(m_Handle, VK_PIPELINE_BIND_POINT_COMPUTE, ((const GraphicsPipeline&)pipeline).GetHandle());
    }

    void CommandBuffer::BindVertexBuffer(const Nova::Buffer& vertexBuffer, const size_t offset)
    {
        const Buffer& vertexBuff = (const Buffer&)vertexBuffer;
        const VkCommandBuffer cmdBuff = GetHandle();
        vkCmdBindVertexBuffers(cmdBuff, 0, 1, vertexBuff.GetHandlePtr(), &offset);
    }

    void CommandBuffer::BindIndexBuffer(const Nova::Buffer& indexBuffer, const size_t offset, const Format indexFormat)
    {
        const Buffer& indexBuff = (const Buffer&)indexBuffer;
        const VkCommandBuffer cmdBuff = GetHandle();
        vkCmdBindIndexBuffer(cmdBuff, indexBuff.GetHandle(), offset, Convert<VkIndexType>(indexFormat));
    }

    void CommandBuffer::BindShaderBindingSet(const Nova::Shader& shader, const Nova::ShaderBindingSet& bindingSet)
    {
        const Shader& sh = (const Shader&)shader;

        VkBindDescriptorSetsInfo info = { VK_STRUCTURE_TYPE_BIND_DESCRIPTOR_SETS_INFO };
        info.layout = sh.GetPipelineLayout();
        info.firstSet = bindingSet.GetSetIndex();
        info.descriptorSetCount = 1;
        info.pDescriptorSets = ((const ShaderBindingSet&)bindingSet).GetHandlePtr();
        info.stageFlags = Convert<VkShaderStageFlags>(sh.GetShaderStageFlags());
        info.dynamicOffsetCount = 0;
        info.pDynamicOffsets = nullptr;
        vkCmdBindDescriptorSets2(m_Handle, &info);
    }

    void CommandBuffer::BindMaterial(const Nova::Material& material)
    {
        BindShaderBindingSet(*material.GetShader(), *material.GetBindingSet());
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

    void CommandBuffer::Draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance)
    {
        vkCmdDraw(m_Handle, vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void CommandBuffer::DrawIndexed(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int32_t vertexOffset, const uint32_t firstInstance)
    {
        vkCmdDrawIndexed(m_Handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void CommandBuffer::DrawIndirect(const Nova::Buffer& buffer, const size_t offset, const uint32_t drawCount)
    {
        vkCmdDrawIndirect(m_Handle, static_cast<const Buffer&>(buffer).GetHandle(), offset, drawCount, sizeof(DrawIndirectParameters));
    }

    void CommandBuffer::DrawIndexedIndirect(const Nova::Buffer& buffer, const uint64_t offset, const uint32_t drawCount)
    {
        vkCmdDrawIndexedIndirect(m_Handle, static_cast<const Buffer&>(buffer).GetHandle(), offset, drawCount, sizeof(DrawIndexedIndirectParameters));
    }

    void CommandBuffer::Dispatch(const uint32_t groupCountX, const uint32_t groupCountY, const uint32_t groupCountZ)
    {
        vkCmdDispatch(m_Handle, groupCountX, groupCountY, groupCountZ);
    }

    void CommandBuffer::DispatchIndirect(const Nova::Buffer& buffer, const size_t offset)
    {
        vkCmdDispatchIndirect(m_Handle, static_cast<const Buffer&>(buffer).GetHandle(), offset);
    }

    void CommandBuffer::PushConstants(const Nova::Shader& shader, const ShaderStageFlags stageFlags, const size_t offset, const size_t size, const void* values)
    {
        vkCmdPushConstants(m_Handle, ((const Shader&)shader).GetPipelineLayout(), Convert<VkShaderStageFlags>(stageFlags), offset, size, values);
    }

    void CommandBuffer::UpdateBuffer(const Nova::Buffer& buffer, const size_t offset, const size_t size, const void* data)
    {
        vkCmdUpdateBuffer(m_Handle, static_cast<const Buffer&>(buffer).GetHandle(), offset, size, data);
    }

    void CommandBuffer::TextureBarrier(const Nova::TextureBarrier& barrier)
    {
        Texture* texture = dynamic_cast<Texture*>(barrier.texture);
        if (!texture) return;

        const VkImageMemoryBarrier vkBarrier = MakeTextureBarrier(barrier);
        const VkPipelineStageFlags srcStageFlags = GetPipelineStageFlags(barrier.sourceAccess);
        const VkPipelineStageFlags dstStageFlags = GetPipelineStageFlags(barrier.destAccess);
        vkCmdPipelineBarrier(m_Handle, srcStageFlags, dstStageFlags, 0, 0, nullptr, 0, nullptr, 1, &vkBarrier);
        texture->SetState(barrier.destState);
    }

    void CommandBuffer::BufferBarrier(const Nova::BufferBarrier& barrier)
    {
        const VkBufferMemoryBarrier vkBarrier = MakeBufferBarrier(barrier);
        vkCmdPipelineBarrier(m_Handle, 0, 0, 0, 0, nullptr, 1, &vkBarrier, 0, nullptr);
    }

    void CommandBuffer::MemoryBarrier(const Nova::MemoryBarrier& memoryBarrier)
    {
        Array<VkImageMemoryBarrier> textureBarriers;
        for (uint32_t i = 0; i < memoryBarrier.textureBarrierCount; i++)
        {
            Nova::TextureBarrier& barrier = memoryBarrier.textureBarriers[i];
            Texture* texture = static_cast<Texture*>(barrier.texture);
            textureBarriers.Add(MakeTextureBarrier(barrier));
            texture->SetState(barrier.destState);
        }

        Array<VkBufferMemoryBarrier> bufferBarriers;
        for (uint32_t i = 0; i < memoryBarrier.bufferBarrierCount; i++)
            bufferBarriers.Add(MakeBufferBarrier(memoryBarrier.bufferBarriers[i]));

        vkCmdPipelineBarrier(m_Handle, 0, 0, 0, 0, nullptr, bufferBarriers.Count(), bufferBarriers.Data(), textureBarriers.Count(), textureBarriers.Data());
    }

    void CommandBuffer::BufferCopy(const Nova::Buffer& src, const Nova::Buffer& dest, const size_t srcOffset, const size_t destOffset, const size_t size)
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

    void CommandBuffer::Blit(const Nova::Texture& src, const BlitRegion& srcRegion, const Nova::Texture& dest, const BlitRegion& destRegion, const Filter filter)
    {
        const Texture& source = static_cast<const Texture&>(src);
        const Texture& destination = static_cast<const Texture&>(dest);
        //NOVA_ASSERT(source.GetFormat() != destination.GetFormat(), "Source and destination textures must have the same format");

        const VkImageLayout sourceInitialLayout = Convert<VkImageLayout>(source.GetState());
        const VkImageLayout destInitialLayout = Convert<VkImageLayout>(destination.GetState());

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
        blitInfo.filter = Convert<VkFilter>(filter);
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

    void CommandBuffer::Blit(const Nova::Texture& src, const Nova::Texture& dest, const Filter filter)
    {
        const BlitRegion srcRegion = { 0, 0, src.GetWidth(), src.GetHeight(), 0 };
        const BlitRegion destRegion = { 0, 0, dest.GetWidth(), dest.GetHeight(), 0 };
        Blit(src, srcRegion, dest, destRegion, filter);
    }

    static VkRenderingAttachmentInfo GetRenderingAttachmentInfo(const RenderPassAttachmentInfo& attachmentInfo)
    {
        const Texture* texture = static_cast<const Texture*>(attachmentInfo.texture);
        const Texture* resolveTexture = static_cast<const Texture*>(attachmentInfo.resolveTexture);

        VkRenderingAttachmentInfo renderingInfo { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        renderingInfo.loadOp = Convert<VkAttachmentLoadOp>(attachmentInfo.loadOp);
        renderingInfo.storeOp = Convert<VkAttachmentStoreOp>(attachmentInfo.storeOp);
        renderingInfo.imageLayout = Convert<VkImageLayout>(attachmentInfo.type);
        renderingInfo.imageView = texture->GetImageView();

        switch (attachmentInfo.type)
        {
        case RenderPassAttachmentType::Color:
            {
                const Color& clearColor = attachmentInfo.clearValue.color;
                renderingInfo.clearValue.color = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
            }
            break;
        case RenderPassAttachmentType::Depth:
            {
                const ClearValue clearValue = attachmentInfo.clearValue;
                renderingInfo.clearValue.depthStencil = { clearValue.depth, clearValue.stencil };
            }
            break;
        }

        if (resolveTexture)
        {
            renderingInfo.resolveMode = Convert<VkResolveModeFlagBits>(attachmentInfo.resolveMode);
            renderingInfo.resolveImageLayout = Convert<VkImageLayout>(attachmentInfo.type);
            renderingInfo.resolveImageView = resolveTexture->GetImageView();
        }

        return renderingInfo;
    }

    void CommandBuffer::BeginRenderPass(const RenderPassBeginInfo& beginInfo)
    {
        if (!s_RenderPass)
        {
            s_RenderPass = new RenderPassBeginInfo(beginInfo);
        }
        Array<VkRenderingAttachmentInfo> colorAttachments;
        VkRenderingAttachmentInfo depthAttachment;
        for (uint32_t i = 0; i < beginInfo.colorAttachmentCount; i++)
        {
            const RenderPassAttachmentInfo& colorAttachmentInfo = beginInfo.colorAttachments[i];
            colorAttachments.Add(GetRenderingAttachmentInfo(colorAttachmentInfo));
        }

        VkRenderingInfo renderingInfo { VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderingInfo.layerCount = 1;
        renderingInfo.viewMask = 0;
        renderingInfo.renderArea.extent = { beginInfo.renderArea.width, beginInfo.renderArea.height };
        renderingInfo.renderArea.offset = { (int32_t)beginInfo.renderArea.x, (int32_t)beginInfo.renderArea.y };
        renderingInfo.colorAttachmentCount = colorAttachments.Count();
        renderingInfo.pColorAttachments = colorAttachments.Data();

        if (beginInfo.depthAttachment)
        {
            depthAttachment = GetRenderingAttachmentInfo(*beginInfo.depthAttachment);
            renderingInfo.pDepthAttachment = &depthAttachment;
        } else
        {
            renderingInfo.pDepthAttachment = nullptr;
        }

        vkCmdBeginRendering(m_Handle, &renderingInfo);
    }

    void CommandBuffer::EndRenderPass()
    {
        vkCmdEndRendering(m_Handle);
        delete s_RenderPass;
        s_RenderPass = nullptr;
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
