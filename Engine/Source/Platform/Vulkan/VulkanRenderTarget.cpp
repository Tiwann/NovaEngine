#include "VulkanRenderTarget.h"
#include "Rendering/Renderer.h"
#include "VulkanRenderer.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFence.h"
#include "Runtime/Log.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>


namespace Nova
{
    VulkanRenderTarget::VulkanRenderTarget(Renderer* Owner) : RenderTarget(Owner)
    {
    }

    /*static const auto ConvertLayout = [](const RenderTargetAttachmentFlagBits Flags) -> VkImageLayout
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

    static const auto ConvertUsage = [](const RenderTargetAttachmentFlagBits Flags) -> VkImageUsageFlags
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

    static const auto ConvertAspect = [](const RenderTargetAttachmentFlagBits Flags) -> VkImageAspectFlags
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


    static const auto ConvertWriteAccess = [](const RenderTargetAttachmentFlagBits Flags) -> VkAccessFlags
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

    static const auto ConvertReadAccess = [](const RenderTargetAttachmentFlagBits Flags) -> VkAccessFlags
    {
        switch (Flags)
        {
        case RenderTargetAttachmentFlagBits::Color: return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        case RenderTargetAttachmentFlagBits::Depth: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        case RenderTargetAttachmentFlagBits::Stencil: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        case RenderTargetAttachmentFlagBits::DepthStencil: return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        default: throw;
        }
    };

    static const auto ConvertPipelineStage = [](const RenderTargetAttachmentFlagBits Flags) -> VkPipelineStageFlags
    {
        switch (Flags)
        {
        case RenderTargetAttachmentFlagBits::Color: return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        case RenderTargetAttachmentFlagBits::Depth: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case RenderTargetAttachmentFlagBits::Stencil: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        case RenderTargetAttachmentFlagBits::DepthStencil: return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        default: throw;
        }
    };*/

    bool VulkanRenderTarget::Initialize(const RenderTargetCreateInfo& CreateInfo)
    {
        m_CachedCreateInfo = CreateInfo;
        m_Width = CreateInfo.Width;
        m_Height = CreateInfo.Height;
        m_SampleCount = CreateInfo.SampleCount;

        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const VulkanRendererTypeConvertor& Convertor = Renderer->Convertor;
        const VmaAllocator Allocator = Renderer->GetAllocator();
        const VkDevice Device = Renderer->GetDevice();
        const VkQueue GraphicsQueue = Renderer->GetGraphicsQueue();
        const u32 GraphicsQueueIndex = Renderer->GetGraphicsQueueFamily();
        CommandPool* CommandPool = Renderer->GetCommandPool();

        Array<VkImageMemoryBarrier> Barriers;

        for (size_t ImageIndex = 0; ImageIndex < Renderer->GetImageCount(); ImageIndex++)
        {
            {
                VkImageCreateInfo ColorImageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
                ColorImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
                ColorImageCreateInfo.extent.width = CreateInfo.Width;
                ColorImageCreateInfo.extent.height = CreateInfo.Height;
                ColorImageCreateInfo.extent.depth = 1;
                ColorImageCreateInfo.format = Convertor.ConvertFormat(CreateInfo.ColorFormat);
                ColorImageCreateInfo.samples = (VkSampleCountFlagBits)CreateInfo.SampleCount;
                ColorImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                ColorImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                ColorImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                ColorImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                ColorImageCreateInfo.pQueueFamilyIndices = &GraphicsQueueIndex;
                ColorImageCreateInfo.queueFamilyIndexCount = 1;
                ColorImageCreateInfo.arrayLayers = 1;
                ColorImageCreateInfo.mipLevels = 1;

                VmaAllocationCreateInfo AllocationCreateInfo{};
                AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                AllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

                VkResult Result = vmaCreateImage(Allocator, &ColorImageCreateInfo, &AllocationCreateInfo,
                    &m_ColorImages[ImageIndex],
                    &m_ColorAllocations[ImageIndex], nullptr);
                if (VK_FAILED(Result))
                    return false;

                VkImageViewCreateInfo ImageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
                ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                ImageViewCreateInfo.format = Convertor.ConvertFormat(CreateInfo.ColorFormat);
                ImageViewCreateInfo.image = m_ColorImages[ImageIndex];
                ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
                ImageViewCreateInfo.subresourceRange.levelCount = 1;
                ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                ImageViewCreateInfo.subresourceRange.layerCount = 1;
                if (VK_FAILED(vkCreateImageView(Device, &ImageViewCreateInfo, nullptr,
                                          &m_ColorImageViews[ImageIndex])))
                {
                    return false;
                }

                VkImageMemoryBarrier Barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
                Barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                Barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                Barrier.image = m_ColorImages[ImageIndex];
                Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                Barrier.subresourceRange.baseMipLevel = 0;
                Barrier.subresourceRange.levelCount = 1;
                Barrier.subresourceRange.baseArrayLayer = 0;
                Barrier.subresourceRange.layerCount = 1;
                Barrier.srcAccessMask = VK_ACCESS_NONE_KHR;
                Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                Barriers.Add(Barrier);
            }



            {
                VkImageCreateInfo DepthImageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
                DepthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
                DepthImageCreateInfo.extent.width = CreateInfo.Width;
                DepthImageCreateInfo.extent.height = CreateInfo.Height;
                DepthImageCreateInfo.extent.depth = CreateInfo.Depth;
                DepthImageCreateInfo.format = Convertor.ConvertFormat(CreateInfo.DepthFormat);
                DepthImageCreateInfo.samples = (VkSampleCountFlagBits)CreateInfo.SampleCount;
                DepthImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                DepthImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                DepthImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                DepthImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                DepthImageCreateInfo.pQueueFamilyIndices = &GraphicsQueueIndex;
                DepthImageCreateInfo.queueFamilyIndexCount = 1;
                DepthImageCreateInfo.arrayLayers = 1;
                DepthImageCreateInfo.mipLevels = 1;

                VmaAllocationCreateInfo AllocationCreateInfo{};
                AllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
                AllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

                VmaAllocationInfo AllocationInfo;
                if (VK_FAILED(vmaCreateImage(Allocator, &DepthImageCreateInfo, &AllocationCreateInfo,
                    &m_DepthImages[ImageIndex],
                    &m_DepthAllocations[ImageIndex], &AllocationInfo)))
                {
                    return false;
                }

                VkImageViewCreateInfo ImageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
                ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                ImageViewCreateInfo.format = Convertor.ConvertFormat(CreateInfo.DepthFormat);
                ImageViewCreateInfo.image = m_DepthImages[ImageIndex];
                ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
                ImageViewCreateInfo.subresourceRange.levelCount = 1;
                ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
                ImageViewCreateInfo.subresourceRange.layerCount = 1;
                if (VK_FAILED(vkCreateImageView(Device, &ImageViewCreateInfo, nullptr,
                                          &m_DepthImageViews[ImageIndex])))
                {
                    return false;
                }

                VkImageMemoryBarrier Barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
                Barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                Barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                Barrier.image = m_DepthImages[ImageIndex];
                Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                Barrier.subresourceRange.baseMipLevel = 0;
                Barrier.subresourceRange.levelCount = 1;
                Barrier.subresourceRange.baseArrayLayer = 0;
                Barrier.subresourceRange.layerCount = 1;
                Barrier.srcAccessMask = VK_ACCESS_NONE_KHR;
                Barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                Barriers.Add(Barrier);
            }
        }


        CommandBuffer* CommandBuffer = CommandPool->AllocateCommandBuffer({CommandBufferLevel::Primary});
        if (CommandBuffer->Begin({CommandBufferUsageFlagBits::OneTimeSubmit}))
        {
            VulkanFence* BarrierFence = (VulkanFence*)m_Owner->CreateFence({FenceCreateFlagBits::None});

            vkCmdPipelineBarrier(
                CommandBuffer->As<VulkanCommandBuffer>()->GetHandle(),
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                0, 0, nullptr, 0, nullptr, Barriers.Count(), Barriers.Data());
            CommandBuffer->End();


            VkSubmitInfo SubmitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
            SubmitInfo.commandBufferCount = 1;
            SubmitInfo.pCommandBuffers = CommandBuffer->As<VulkanCommandBuffer>()->GetHandlePtr();

            vkQueueSubmit(GraphicsQueue, 1, &SubmitInfo, BarrierFence->GetHandle());
            vkWaitForFences(Device, 1, BarrierFence->GetHandlePtr(), true, U64_MAX);
            BarrierFence->Destroy();
            delete BarrierFence;
            CommandPool->FreeCommandBuffer(CommandBuffer);
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
        Renderer->WaitIdle();

        for (size_t ImageIndex = 0; ImageIndex < ImageCount; ImageIndex++)
        {
            vkDestroyImageView(Device, m_ColorImageViews[ImageIndex], nullptr);
            vkDestroyImageView(Device, m_DepthImageViews[ImageIndex], nullptr);
            vmaDestroyImage(Allocator, m_ColorImages[ImageIndex], m_ColorAllocations[ImageIndex]);
            vmaDestroyImage(Allocator, m_DepthImages[ImageIndex], m_DepthAllocations[ImageIndex]);
        }
    }

    void VulkanRenderTarget::BeginRendering(CommandBuffer* Cmd) const
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const u32 FrameIndex = Renderer->GetCurrentFrameIndex();

        VkImageMemoryBarrier ColorBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        ColorBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ColorBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        ColorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ColorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ColorBarrier.image = m_ColorImages[FrameIndex];
        ColorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ColorBarrier.subresourceRange.baseMipLevel = 0;
        ColorBarrier.subresourceRange.levelCount = 1;
        ColorBarrier.subresourceRange.baseArrayLayer = 0;
        ColorBarrier.subresourceRange.layerCount = 1;
        ColorBarrier.srcAccessMask = VK_ACCESS_NONE_KHR;
        ColorBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(
            Cmd->As<VulkanCommandBuffer>()->GetHandle(),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, nullptr, 0, nullptr, 1, &ColorBarrier
        );

        VkRenderingAttachmentInfo ColorAttachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        ColorAttachment.clearValue.color = VkClearColorValue{ { 0.0f, 0.0, 0.0, 1.0f } };
        ColorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        ColorAttachment.imageView = m_ColorImageViews[FrameIndex];
        ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingAttachmentInfo DepthAttachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        DepthAttachment.clearValue.depthStencil = VkClearDepthStencilValue(1.0f, 0);
        DepthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        DepthAttachment.imageView = m_DepthImageViews[FrameIndex];
        DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfo RenderingInfo = { VK_STRUCTURE_TYPE_RENDERING_INFO };
        RenderingInfo.layerCount = 1;
        RenderingInfo.renderArea.extent = VkExtent2D{ m_Width, m_Height };
        RenderingInfo.renderArea.offset = VkOffset2D{ 0, 0 };
        RenderingInfo.viewMask = 0;
        RenderingInfo.colorAttachmentCount = 1;
        RenderingInfo.pColorAttachments = &ColorAttachment;
        RenderingInfo.pDepthAttachment = &DepthAttachment;

        vkCmdBeginRendering(Cmd->As<VulkanCommandBuffer>()->GetHandle(), &RenderingInfo);
    }

    void VulkanRenderTarget::EndRendering(CommandBuffer* Cmd) const
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const u32 FrameIndex = Renderer->GetCurrentFrameIndex();

        vkCmdEndRendering(Cmd->As<VulkanCommandBuffer>()->GetHandle());

        VkImageMemoryBarrier ColorBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        ColorBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        ColorBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        ColorBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        ColorBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        ColorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ColorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ColorBarrier.image = m_ColorImages[FrameIndex];
        ColorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ColorBarrier.subresourceRange.baseMipLevel = 0;
        ColorBarrier.subresourceRange.levelCount = 1;
        ColorBarrier.subresourceRange.baseArrayLayer = 0;
        ColorBarrier.subresourceRange.layerCount = 1;


        vkCmdPipelineBarrier(
            Cmd->As<VulkanCommandBuffer>()->GetHandle(),
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &ColorBarrier
        );
    }

    VkImage VulkanRenderTarget::GetColorImage() const
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const size_t ImageIndex = Renderer->GetCurrentFrameIndex();
        return m_ColorImages[ImageIndex];
    }

    VkImageView VulkanRenderTarget::GetColorImageView() const
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const size_t ImageIndex = Renderer->GetCurrentFrameIndex();
        return m_ColorImageViews[ImageIndex];
    }

    VkImage VulkanRenderTarget::GetDepthImage() const
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const size_t ImageIndex = Renderer->GetCurrentFrameIndex();
        return m_DepthImages[ImageIndex];
    }

    VkImageView VulkanRenderTarget::GetDepthImageView() const
    {
        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const size_t ImageIndex = Renderer->GetCurrentFrameIndex();
        return m_DepthImageViews[ImageIndex];
    }
}
