#include "RenderTarget.h"
#include "Device.h"
#include "Conversions.h"
#include "CommandBuffer.h"
#include "Rendering/CommandBuffer.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>


namespace Nova::Vulkan
{
    bool RenderTarget::Initialize(const Rendering::RenderTargetCreateInfo& createInfo)
    {
        Device* device = (Device*)createInfo.device;
        Swapchain* swapchain = device->GetSwapchain();
        CommandPool* commandPool = device->GetCommandPool();
        const VkDevice deviceHandle = device->GetHandle();
        const VmaAllocator allocatorHandle = device->GetAllocator();
        const Queue* graphicsQueue = device->GetGraphicsQueue();

        Array<VkImageMemoryBarrier> barriers;

        for (size_t imageIndex = 0; imageIndex < swapchain->GetImageCount(); imageIndex++)
        {
            VkImageCreateInfo colorImageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
            colorImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
            colorImageCreateInfo.extent.width = createInfo.width;
            colorImageCreateInfo.extent.height = createInfo.height;
            colorImageCreateInfo.extent.depth = createInfo.depth;
            colorImageCreateInfo.format = Convert<Format, VkFormat>(createInfo.colorFormat);
            colorImageCreateInfo.samples = (VkSampleCountFlagBits)createInfo.sampleCount;
            colorImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            colorImageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            colorImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            colorImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorImageCreateInfo.pQueueFamilyIndices = graphicsQueue->GetIndexPtr();
            colorImageCreateInfo.queueFamilyIndexCount = 1;
            colorImageCreateInfo.arrayLayers = 1;
            colorImageCreateInfo.mipLevels = 1;

            VmaAllocationCreateInfo colorImageAllocationCreateInfo{};
            colorImageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            colorImageAllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

            if (vmaCreateImage(allocatorHandle,
                &colorImageCreateInfo,
                &colorImageAllocationCreateInfo,
                &m_ColorImages[imageIndex],
                &m_ColorAllocations[imageIndex], nullptr) != VK_SUCCESS)
                return false;

            VkImageViewCreateInfo colorImageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            colorImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            colorImageViewCreateInfo.format = Convert<Format, VkFormat>(createInfo.colorFormat);
            colorImageViewCreateInfo.image = m_ColorImages[imageIndex];
            colorImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            colorImageViewCreateInfo.subresourceRange.levelCount = 1;
            colorImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            colorImageViewCreateInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(deviceHandle,
                &colorImageViewCreateInfo,
                nullptr,&m_ColorImageViews[imageIndex]) != VK_SUCCESS)
                return false;

            VkImageMemoryBarrier colorBarrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
            colorBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            colorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            colorBarrier.image = m_ColorImages[imageIndex];
            colorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorBarrier.subresourceRange.baseMipLevel = 0;
            colorBarrier.subresourceRange.levelCount = 1;
            colorBarrier.subresourceRange.baseArrayLayer = 0;
            colorBarrier.subresourceRange.layerCount = 1;
            colorBarrier.srcAccessMask = VK_ACCESS_NONE_KHR;
            colorBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barriers.Add(colorBarrier);


            VkImageCreateInfo depthImageCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
            depthImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
            depthImageCreateInfo.extent.width = createInfo.width;
            depthImageCreateInfo.extent.height = createInfo.height;
            depthImageCreateInfo.extent.depth = createInfo.depth;
            depthImageCreateInfo.format = Convert<Format, VkFormat>(createInfo.depthFormat);
            depthImageCreateInfo.samples = (VkSampleCountFlagBits)createInfo.sampleCount;
            depthImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            depthImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            depthImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            depthImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthImageCreateInfo.pQueueFamilyIndices = graphicsQueue->GetIndexPtr();
            depthImageCreateInfo.queueFamilyIndexCount = 1;
            depthImageCreateInfo.arrayLayers = 1;
            depthImageCreateInfo.mipLevels = 1;

            VmaAllocationCreateInfo depthImageAllocationCreateInfo{};
            depthImageAllocationCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            depthImageAllocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

            if (vmaCreateImage(allocatorHandle,
                &depthImageCreateInfo,
                &depthImageAllocationCreateInfo,
                &m_DepthImages[imageIndex],
                &m_DepthAllocations[imageIndex], nullptr) != VK_SUCCESS)
                return false;

            VkImageViewCreateInfo depthImageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            depthImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            depthImageViewCreateInfo.format = Convert<Format, VkFormat>(createInfo.depthFormat);
            depthImageViewCreateInfo.image = m_DepthImages[imageIndex];
            depthImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            depthImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
            depthImageViewCreateInfo.subresourceRange.levelCount = 1;
            depthImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            depthImageViewCreateInfo.subresourceRange.layerCount = 1;
            if (vkCreateImageView(deviceHandle,
                &depthImageViewCreateInfo,
                nullptr, &m_DepthImageViews[imageIndex]))
                return false;

            VkImageMemoryBarrier depthBarrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
            depthBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            depthBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            depthBarrier.image = m_DepthImages[imageIndex];
            depthBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            depthBarrier.subresourceRange.baseMipLevel = 0;
            depthBarrier.subresourceRange.levelCount = 1;
            depthBarrier.subresourceRange.baseArrayLayer = 0;
            depthBarrier.subresourceRange.layerCount = 1;
            depthBarrier.srcAccessMask = VK_ACCESS_NONE_KHR;
            depthBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            barriers.Add(depthBarrier);
        }

        CommandBuffer commandBuffer = commandPool->AllocateCommandBuffer(Rendering::CommandBufferLevel::Primary);

        if (commandBuffer.Begin({Rendering::CommandBufferUsageFlagBits::OneTimeSubmit}))
        {
            Rendering::FenceCreateInfo fenceCreateInfo;
            fenceCreateInfo.device = device;
            fenceCreateInfo.flags = Rendering::FenceCreateFlagBits::None;

            Fence fence;
            if (!fence.Initialize(fenceCreateInfo))
                return false;

            vkCmdPipelineBarrier(
                commandBuffer.GetHandle(),
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                0, 0, nullptr, 0, nullptr, barriers.Count(), barriers.Data());

            commandBuffer.End();

            graphicsQueue->Submit(&commandBuffer, nullptr, nullptr, &fence, 0);

            fence.Wait(~0);
            fence.Destroy();
            commandBuffer.Free();
        }

        m_Device = createInfo.device;
        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_Depth = createInfo.depth;
        m_SampleCount = createInfo.sampleCount;
        m_ColorFormat = createInfo.colorFormat;
        m_DepthFormat = createInfo.depthFormat;
        m_ImageCount = swapchain->GetImageCount();
        return true;
    }

    void RenderTarget::Destroy()
    {
        Device* device = static_cast<Device*>(m_Device);
        const Swapchain* swapchain = device->GetSwapchain();
        const VkDevice deviceHandle = device->GetHandle();
        const VmaAllocator allocatorHandle = device->GetAllocator();

        device->WaitIdle();
        for (size_t imageIndex = 0; imageIndex < swapchain->GetImageCount(); imageIndex++)
        {
            vkDestroyImageView(deviceHandle, m_ColorImageViews[imageIndex], nullptr);
            vkDestroyImageView(deviceHandle, m_DepthImageViews[imageIndex], nullptr);
            vmaDestroyImage(allocatorHandle, m_ColorImages[imageIndex], m_ColorAllocations[imageIndex]);
            vmaDestroyImage(allocatorHandle, m_DepthImages[imageIndex], m_DepthAllocations[imageIndex]);
        }
    }

    void RenderTarget::BeginRendering(Rendering::CommandBuffer& commandBuffer)
    {
        m_CommandBuffer = static_cast<CommandBuffer*>(&commandBuffer);
        const Device* device = static_cast<Device*>(m_Device);
        const uint32_t frameIndex = device->GetCurrentFrameIndex();

        VkImageMemoryBarrier colorBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        colorBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        colorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        colorBarrier.image = m_ColorImages[frameIndex];
        colorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorBarrier.subresourceRange.baseMipLevel = 0;
        colorBarrier.subresourceRange.levelCount = 1;
        colorBarrier.subresourceRange.baseArrayLayer = 0;
        colorBarrier.subresourceRange.layerCount = 1;
        colorBarrier.srcAccessMask = VK_ACCESS_NONE_KHR;
        colorBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(
            m_CommandBuffer->GetHandle(),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            0, 0, nullptr, 0, nullptr, 1, &colorBarrier
        );

        VkRenderingAttachmentInfo colorAttachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        colorAttachment.clearValue.color = VkClearColorValue{ { 0.0f, 0.0, 0.0, 1.0f } };
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.imageView = m_ColorImageViews[frameIndex];
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingAttachmentInfo depthAttachment = { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
        depthAttachment.clearValue.depthStencil = VkClearDepthStencilValue(1.0f, 0);
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.imageView = m_DepthImageViews[frameIndex];
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

        VkRenderingInfo renderingInfo = { VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderingInfo.layerCount = 1;
        renderingInfo.renderArea.extent = VkExtent2D{ m_Width, m_Height };
        renderingInfo.renderArea.offset = VkOffset2D{ 0, 0 };
        renderingInfo.viewMask = 0;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &colorAttachment;
        renderingInfo.pDepthAttachment = &depthAttachment;

        vkCmdBeginRendering(m_CommandBuffer->GetHandle(), &renderingInfo);
    }

    void RenderTarget::EndRendering()
    {
        const Device* device = static_cast<Device*>(m_Device);
        const uint32_t frameIndex = device->GetCurrentFrameIndex();

        vkCmdEndRendering(m_CommandBuffer->GetHandle());

        VkImageMemoryBarrier colorBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        colorBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        colorBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        colorBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        colorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        colorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        colorBarrier.image = m_ColorImages[frameIndex];
        colorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorBarrier.subresourceRange.baseMipLevel = 0;
        colorBarrier.subresourceRange.levelCount = 1;
        colorBarrier.subresourceRange.baseArrayLayer = 0;
        colorBarrier.subresourceRange.layerCount = 1;


        vkCmdPipelineBarrier(
            m_CommandBuffer->GetHandle(),
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &colorBarrier
        );
    }

    bool RenderTarget::Resize(const uint32_t newWidth, const uint32_t newHeight)
    {
        if (m_Width == newWidth && m_Height == newHeight)
            return true;

        if (newWidth == 0 || newHeight == 0)
            return false;

        Destroy();

        Rendering::RenderTargetCreateInfo createInfo;
        createInfo.device = m_Device;
        createInfo.width = newWidth;
        createInfo.height = newHeight;
        createInfo.depth = m_Depth;
        createInfo.colorFormat = m_ColorFormat;
        createInfo.depthFormat = m_DepthFormat;
        createInfo.sampleCount = m_SampleCount;
        return Initialize(createInfo);
    }

    void RenderTarget::Clear(const Color& color)
    {
        const VkCommandBuffer commandBuffer = m_CommandBuffer->GetHandle();

        VkClearAttachment clearAttachment;
        clearAttachment.colorAttachment = 0;
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        clearAttachment.clearValue.color = VkClearColorValue{ { color.r, color.g, color.b, color.a }};

        VkClearRect clearRect;
        clearRect.rect.extent = VkExtent2D{ m_Width, m_Height };
        clearRect.rect.offset = VkOffset2D{ 0, 0 };
        clearRect.layerCount = 1;
        clearRect.baseArrayLayer = 0;

        vkCmdClearAttachments(commandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    void RenderTarget::Clear(const float depth, const uint8_t stencil)
    {
        const VkCommandBuffer commandBuffer = m_CommandBuffer->GetHandle();

        VkClearAttachment clearAttachment;
        clearAttachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        clearAttachment.clearValue.depthStencil = VkClearDepthStencilValue{ depth, stencil };

        VkClearRect clearRect;
        clearRect.rect.offset = VkOffset2D{ 0, 0 };
        clearRect.rect.extent = VkExtent2D{ m_Width, m_Height };
        clearRect.layerCount = 1;
        clearRect.baseArrayLayer = 0;

        vkCmdClearAttachments(commandBuffer, 1, &clearAttachment, 1, &clearRect);
    }

    const Texture& RenderTarget::GetColorTexture()
    {
        const size_t imageIndex = ((Device*)m_Device)->GetCurrentFrameIndex();
        const auto createTexture = [this, &imageIndex]() -> Texture
        {
            Texture texture;
            texture.m_Width = m_Width;
            texture.m_Height = m_Height;
            texture.m_Image = m_ColorImages[imageIndex];
            texture.m_ImageView = m_ColorImageViews[imageIndex];
            texture.m_SampleCount = m_SampleCount;
            texture.m_Device = (Device*)m_Device;
            texture.m_Format = m_ColorFormat;
            texture.m_Allocation = m_ColorAllocations[imageIndex];
            texture.m_Mips = 1;
            texture.m_UsageFlags = Rendering::TextureUsageFlagBits::Attachment;
            texture.m_SampleCount = m_SampleCount;
            return texture;
        };

        return m_ColorTexture.Get(createTexture);
    }

    const Texture& RenderTarget::GetDepthTexture()
    {
        const size_t imageIndex = ((Device*)m_Device)->GetCurrentFrameIndex();
        const auto createTexture = [this, &imageIndex]() -> Texture
        {
            Texture texture;
            texture.m_Width = m_Width;
            texture.m_Height = m_Height;
            texture.m_Image = m_DepthImages[imageIndex];
            texture.m_ImageView = m_DepthImageViews[imageIndex];
            texture.m_SampleCount = m_SampleCount;
            texture.m_Device = (Device*)m_Device;
            texture.m_Format = m_DepthFormat;
            texture.m_Allocation = m_DepthAllocations[imageIndex];
            texture.m_Mips = 1;
            texture.m_UsageFlags = Rendering::TextureUsageFlagBits::Attachment;
            texture.m_SampleCount = m_SampleCount;
            return texture;
        };

        return m_DepthTexture.Get(createTexture);
    }


    VkImage RenderTarget::GetColorImage(size_t index) const
    {
        return m_ColorImages[index];
    }

    VkImageView RenderTarget::GetColorImageView(size_t index) const
    {
        return m_ColorImageViews[index];
    }

    VmaAllocation RenderTarget::GetColorImageAllocation(size_t index) const
    {
        return m_ColorAllocations[index];
    }

    VkImage RenderTarget::GetDepthImage(size_t index) const
    {
        return m_DepthImages[index];
    }

    VkImageView RenderTarget::GetDepthImageView(size_t index) const
    {
        return m_DepthImageViews[index];
    }

    VmaAllocation RenderTarget::GetDepthImageAllocation(size_t index) const
    {
        return m_DepthAllocations[index];
    }
}
