#include "VulkanRenderTarget.h"
#include "Rendering/Renderer.h"
#include "VulkanRenderer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "Runtime/Log.h"

#include <vulkan/vulkan.h>


namespace Nova
{
    VulkanRenderTarget::VulkanRenderTarget(Renderer* Owner) : RenderTarget(Owner)
    {
    }

    bool VulkanRenderTarget::Initialize(const RenderTargetCreateInfo& CreateInfo)
    {
        m_CachedCreateInfo = CreateInfo;
        m_Width = CreateInfo.Width;
        m_Height = CreateInfo.Height;

        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const VulkanRendererTypeConvertor& Convertor = Renderer->Convertor;
        const VmaAllocator Allocator = Renderer->GetAllocator();
        const VkDevice Device = Renderer->GetDevice();
        const VkQueue GraphicsQueue = Renderer->GetGraphicsQueue();
        const u32 GraphicsQueueIndex = Renderer->GetGraphicsQueueFamily();
        CommandPool* CommandPool = Renderer->GetCommandPool();

        const auto ConvertUsage = [](const RenderTargetAttachmentFlagBits Flags) -> VkImageUsageFlags
        {
            switch (Flags)
            {
            case RenderTargetAttachmentFlagBits::Color: return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            case RenderTargetAttachmentFlagBits::Depth: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            case RenderTargetAttachmentFlagBits::Stencil: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            case RenderTargetAttachmentFlagBits::DepthStencil: return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            default: throw;
            }
        };

        const auto ConvertAspect = [](const RenderTargetAttachmentFlagBits Flags) -> VkImageAspectFlags
        {
            switch (Flags)
            {
            case RenderTargetAttachmentFlagBits::Color: return VK_IMAGE_ASPECT_COLOR_BIT;
            case RenderTargetAttachmentFlagBits::Depth: return VK_IMAGE_ASPECT_DEPTH_BIT;
            case RenderTargetAttachmentFlagBits::Stencil: return VK_IMAGE_ASPECT_STENCIL_BIT;
            case RenderTargetAttachmentFlagBits::DepthStencil: return VK_IMAGE_ASPECT_DEPTH_BIT |
                    VK_IMAGE_ASPECT_STENCIL_BIT;
            default: throw;
            }
        };

        const auto ConvertLayout = [](const RenderTargetAttachmentFlagBits Flags) -> VkImageLayout
        {
            switch (Flags)
            {
            case RenderTargetAttachmentFlagBits::Color: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case RenderTargetAttachmentFlagBits::Depth: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            case RenderTargetAttachmentFlagBits::Stencil: return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
            case RenderTargetAttachmentFlagBits::DepthStencil: return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            default: throw;
            }
        };

        const auto ConvertAccess = [](const RenderTargetAttachmentFlagBits Flags) -> VkAccessFlags
        {
            switch (Flags)
            {
            case RenderTargetAttachmentFlagBits::Color: return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            case RenderTargetAttachmentFlagBits::Depth: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            case RenderTargetAttachmentFlagBits::Stencil: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            case RenderTargetAttachmentFlagBits::DepthStencil: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            default: throw;
            }
        };

        const auto ConvertPipelineStage = [](const RenderTargetAttachmentFlagBits Flags) -> VkPipelineStageFlags
        {
            switch (Flags)
            {
            case RenderTargetAttachmentFlagBits::Color: return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            case RenderTargetAttachmentFlagBits::Depth: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            case RenderTargetAttachmentFlagBits::Stencil: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            case RenderTargetAttachmentFlagBits::DepthStencil: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            default: throw;
            }
        };

        for (size_t AttachmentIndex = 0; AttachmentIndex < CreateInfo.AttachmentInfos.Count(); AttachmentIndex++)
        {
            const RenderTargetAttachmentInfo& AttachmentCreateInfo = CreateInfo.AttachmentInfos[AttachmentIndex];
            VulkanRenderTargetAttachment* Attachment = new VulkanRenderTargetAttachment();
            Attachment->Name = AttachmentCreateInfo.Name;
            Attachment->Format = AttachmentCreateInfo.Format;

            for (size_t ImageIndex = 0; ImageIndex < Renderer->GetImageCount(); ImageIndex++)
            {
                VkImageCreateInfo ImageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
                ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
                ImageCreateInfo.extent.width = CreateInfo.Width;
                ImageCreateInfo.extent.height = CreateInfo.Height;
                ImageCreateInfo.extent.depth = 1;
                ImageCreateInfo.format = Convertor.ConvertFormat(Attachment->Format);
                ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                ImageCreateInfo.usage = ConvertUsage(AttachmentCreateInfo.AttachmentType);
                ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                ImageCreateInfo.pQueueFamilyIndices = &GraphicsQueueIndex;
                ImageCreateInfo.queueFamilyIndexCount = 1;
                ImageCreateInfo.arrayLayers = 1;
                ImageCreateInfo.mipLevels = 1;
                if (AttachmentCreateInfo.AttachmentType == RenderTargetAttachmentFlagBits::Depth ||
                    AttachmentCreateInfo.AttachmentType == RenderTargetAttachmentFlagBits::Stencil ||
                    AttachmentCreateInfo.AttachmentType == RenderTargetAttachmentFlagBits::DepthStencil)
                {
                    ImageCreateInfo.extent.depth = CreateInfo.Depth;
                }

                VmaAllocationCreateInfo AllocationCreateInfo{};
                AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                AllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

                VmaAllocationInfo AllocationInfo;
                VkResult Result = vmaCreateImage(Allocator, &ImageCreateInfo, &AllocationCreateInfo,
                                                 &Attachment->Images[ImageIndex],
                                                 &Attachment->Allocations[ImageIndex], &AllocationInfo);
                if (VK_FAILED(Result))
                {
                    NOVA_VULKAN_ERROR("Failed to create Render Target image!");
                    return false;
                }

                VkImageViewCreateInfo ImageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
                ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                ImageViewCreateInfo.format = Convertor.ConvertFormat(Attachment->Format);
                ImageViewCreateInfo.image = Attachment->Images[ImageIndex];
                ImageViewCreateInfo.subresourceRange.aspectMask = ConvertAspect(AttachmentCreateInfo.AttachmentType);
                ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
                ImageViewCreateInfo.subresourceRange.levelCount = 1;
                ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                ImageViewCreateInfo.subresourceRange.layerCount = 1;
                Result = vkCreateImageView(Device, &ImageViewCreateInfo, nullptr,
                                           &Attachment->ImageViews[ImageIndex]);
                if (VK_FAILED(Result))
                {
                    NOVA_VULKAN_ERROR("Failed to create Render Target image view!");
                    return false;
                }

                VkImageMemoryBarrier Barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
                Barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                Barrier.newLayout = ConvertLayout(AttachmentCreateInfo.AttachmentType);
                Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                Barrier.image = Attachment->Images[ImageIndex];
                Barrier.subresourceRange.aspectMask = ConvertAspect(AttachmentCreateInfo.AttachmentType);
                Barrier.subresourceRange.baseMipLevel = 0;
                Barrier.subresourceRange.levelCount = 1;
                Barrier.subresourceRange.baseArrayLayer = 0;
                Barrier.subresourceRange.layerCount = 1;
                Barrier.srcAccessMask = VK_ACCESS_NONE_KHR;
                Barrier.dstAccessMask = ConvertAccess(AttachmentCreateInfo.AttachmentType);

                CommandBuffer* CommandBuffer = CommandPool->AllocateCommandBuffer({CommandBufferLevel::Primary});
                if (CommandBuffer->Begin({CommandBufferUsageFlagBits::OneTimeSubmit}))
                {
                    VkFence BarrierFence = nullptr;
                    VkFenceCreateInfo FenceCreateInfo = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
                    vkCreateFence(Device, &FenceCreateInfo, nullptr, &BarrierFence);

                    vkCmdPipelineBarrier(
                        CommandBuffer->As<VulkanCommandBuffer>()->GetHandle(),
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        ConvertPipelineStage(AttachmentCreateInfo.AttachmentType),
                        0, 0, nullptr, 0, nullptr, 1, &Barrier);
                    CommandBuffer->End();


                    VkSubmitInfo SubmitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
                    SubmitInfo.commandBufferCount = 1;
                    SubmitInfo.pCommandBuffers = CommandBuffer->As<VulkanCommandBuffer>()->GetHandlePtr();

                    vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, BarrierFence);
                    vkWaitForFences(Device, 1, &BarrierFence, true, U64_MAX);
                    vkDestroyFence(Device, BarrierFence, nullptr);
                    CommandPool->FreeCommandBuffer(CommandBuffer);
                }
            }
            m_Attachments.Add(Attachment);
        }
        return true;
    }

    bool VulkanRenderTarget::Resize(const u32 Width, const u32 Height)
    {
        if (m_Width == Width && m_Height == Height)
            return true;

        Destroy();
        RenderTargetCreateInfo CreateInfo = m_CachedCreateInfo;
        CreateInfo.Width = Width;
        CreateInfo.Height = Height;
        m_Width = Width;
        m_Height = Height;
        return Initialize(CreateInfo);
    }

    void VulkanRenderTarget::Destroy()
    {
        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const size_t ImageCount = Renderer->GetImageCount();
        const VkDevice Device = Renderer->GetDevice();
        const VmaAllocator Allocator = Renderer->GetAllocator();
        vkDeviceWaitIdle(Device);

        for (size_t AttachmentIndex = 0; AttachmentIndex < m_Attachments.Count(); AttachmentIndex++)
        {
            const VulkanRenderTargetAttachment* Attachment = (VulkanRenderTargetAttachment*)m_Attachments[AttachmentIndex];
            for (size_t ImageIndex = 0; ImageIndex < ImageCount; ImageIndex++)
            {
                vkDestroyImageView(Device, Attachment->ImageViews[ImageIndex], nullptr);
                vmaDestroyImage(Allocator, Attachment->Images[ImageIndex], Attachment->Allocations[ImageIndex]);
            }
            delete Attachment;
        }
        m_Attachments.Clear();
    }

    void VulkanRenderTarget::BeginRendering()
    {
        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        CommandBuffer* CommandBuffer = Renderer->GetCurrentCommandBuffer();
        VkCommandBuffer Cmd = CommandBuffer->As<VulkanCommandBuffer>()->GetHandle();

        /*VkImageMemoryBarrier ColorBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        ColorBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ColorBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        ColorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ColorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ColorBarrier.image = m_Frames[m_CurrentFrameIndex].ColorImage;
        ColorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ColorBarrier.subresourceRange.baseMipLevel = 0;
        ColorBarrier.subresourceRange.levelCount = 1;
        ColorBarrier.subresourceRange.baseArrayLayer = 0;
        ColorBarrier.subresourceRange.layerCount = 1;
        ColorBarrier.srcAccessMask = VK_ACCESS_NONE_KHR;
        ColorBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        const VkImageMemoryBarrier Barriers[] { ColorBarrier };

        vkCmdPipelineBarrier(
            CommandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, nullptr, 0, nullptr, std::size(Barriers), Barriers
        );

        VkRenderingAttachmentInfo ColorAttachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        ColorAttachment.clearValue.color = VkClearColorValue{ { 0.0f, 0.0, 0.0, 1.0f } };
        ColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        ColorAttachment.imageView = m_Frames[m_CurrentFrameIndex].ColorImageView;
        ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingAttachmentInfo DepthAttachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        DepthAttachment.clearValue.depthStencil = VkClearDepthStencilValue(1.0f, 0);
        DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        DepthAttachment.imageView = m_Frames[m_CurrentFrameIndex].DepthImageView;
        DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;


        VkRenderingInfo RenderingInfo = { VK_STRUCTURE_TYPE_RENDERING_INFO };
        RenderingInfo.layerCount = 1;
        RenderingInfo.renderArea.extent = VkExtent2D{ WindowWidth, WindowHeight };
        RenderingInfo.renderArea.offset = VkOffset2D{ 0, 0 };
        RenderingInfo.viewMask = 0;
        RenderingInfo.colorAttachmentCount = 1;
        RenderingInfo.pColorAttachments = &ColorAttachment;
        RenderingInfo.pDepthAttachment = &DepthAttachment;*/

//        vkCmdBeginRendering(Cmd, &RenderingInfo);
    }

    void VulkanRenderTarget::EndRendering()
    {
    }
}
