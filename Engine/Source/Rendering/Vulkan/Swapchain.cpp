#include "Swapchain.h"
#include "Device.h"
#include "Conversions.h"
#include "Containers/StringFormat.h"
#include <vulkan/vulkan.h>

#include "CommandBuffer.h"
#include "RenderTarget.h"


namespace Nova::Vulkan
{
    bool Swapchain::Initialize(const Rendering::SwapchainCreateInfo& createInfo)
    {
        Device* device = dynamic_cast<Device*>(createInfo.device);
        const Surface* surface = dynamic_cast<const Surface*>(createInfo.surface);
        const VkSurfaceKHR surfaceHandle = surface->GetHandle();
        const VkDevice deviceHandle = device->GetHandle();
        const Queue* graphicsQueue = device->GetGraphicsQueue();
        const Queue* presentQueue = device->GetPresentQueue();
        const uint32_t graphicsQueueFamily = graphicsQueue->GetIndex();
        const uint32_t presentQueueFamily = presentQueue->GetIndex();

        VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = surfaceHandle;
        swapchainCreateInfo.imageFormat = Convert<Format, VkFormat>(createInfo.format);
        swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapchainCreateInfo.presentMode = Convert<PresentMode, VkPresentModeKHR>(createInfo.presentMode);
        swapchainCreateInfo.clipped = true;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.imageExtent.width = createInfo.width;
        swapchainCreateInfo.imageExtent.height = createInfo.height;
        swapchainCreateInfo.minImageCount = (uint32_t)createInfo.buffering;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.oldSwapchain = createInfo.recycle ? m_Handle : nullptr;

        if (graphicsQueueFamily == presentQueueFamily)
        {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 1;
            swapchainCreateInfo.pQueueFamilyIndices = &graphicsQueueFamily;
        }
        else
        {
            const uint32_t queueIndices[2] = { graphicsQueueFamily, presentQueueFamily };
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = queueIndices;
        }

        VkSwapchainKHR swapchainHandle = nullptr;
        if (vkCreateSwapchainKHR(deviceHandle, &swapchainCreateInfo, nullptr, &swapchainHandle) != VK_SUCCESS)
        {
            std::println(std::cerr, "Failed to create swapchain!");
            return false;
        }

        vkDestroySwapchainKHR(deviceHandle, m_Handle, nullptr);
        m_Handle = swapchainHandle;


        m_Device = createInfo.device;
        m_Surface = createInfo.surface;
        m_Buffering = createInfo.buffering;
        m_ImageWidth = createInfo.width;
        m_ImageHeight = createInfo.height;
        m_ImageFormat = createInfo.format;
        m_ImagePresentMode = createInfo.presentMode;

        if (vkGetSwapchainImagesKHR(deviceHandle, m_Handle, (uint32_t*)&m_Buffering, m_Images) != VK_SUCCESS)
            return false;

        for (size_t i = 0; i < GetImageCount(); i++)
        {
            vkDestroyImageView(deviceHandle, m_ImageViews[i], nullptr);
            VkImageViewCreateInfo ImageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ImageViewCreateInfo.format = Convert<Format, VkFormat>(m_ImageFormat);
            ImageViewCreateInfo.image = m_Images[i];
            ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ImageViewCreateInfo.subresourceRange.layerCount = 1;
            ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            ImageViewCreateInfo.subresourceRange.levelCount = 1;
            ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;

            vkCreateImageView(deviceHandle, &ImageViewCreateInfo, nullptr, &m_ImageViews[i]);
        }

        m_Valid = true;
        return true;
    }

    void Swapchain::Destroy()
    {
    }

    bool Swapchain::Recreate()
    {
        const Rendering::Surface* surface = GetSurface();
        Rendering::SwapchainCreateInfo createInfo;
        createInfo.device = m_Device;
        createInfo.surface = m_Surface;
        createInfo.width = surface->GetWidth();
        createInfo.height = surface->GetHeight();
        createInfo.format = m_ImageFormat;
        createInfo.buffering = m_Buffering;
        createInfo.presentMode = m_ImagePresentMode;
        createInfo.recycle = true;
        return Initialize(createInfo);
    }

    uint32_t Swapchain::AcquireNextImage(const Semaphore* semaphore)
    {
        uint32_t nextFrameIndex = 0;
        const Device* device = dynamic_cast<Device*>(m_Device);
        const VkDevice deviceHandle = device->GetHandle();
        if (vkAcquireNextImageKHR(deviceHandle, m_Handle, 0xFF, semaphore->GetHandle(), nullptr, &nextFrameIndex) != VK_SUCCESS)
            return 0xFFFFFFFF;
        return nextFrameIndex;
    }

    void Swapchain::ResolveImage(const CommandBuffer* commandBuffer, const RenderTarget* renderTarget)
    {
        const Device* device = dynamic_cast<Device*>(m_Device);
        const uint32_t frameIndex = device->GetCurrentFrameIndex();

        VkImageMemoryBarrier transferBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        transferBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        transferBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        transferBarrier.srcAccessMask = 0;
        transferBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        transferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        transferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        transferBarrier.image = m_Images[frameIndex];
        transferBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        transferBarrier.subresourceRange.baseMipLevel = 0;
        transferBarrier.subresourceRange.levelCount = 1;
        transferBarrier.subresourceRange.baseArrayLayer = 0;
        transferBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer->GetHandle(),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &transferBarrier
        );

        VkImageResolve region;
        region.extent.width = m_ImageWidth;
        region.extent.height = m_ImageHeight;
        region.extent.depth = 1;
        region.dstOffset = VkOffset3D{ 0, 0, 0 };
        region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.dstSubresource.baseArrayLayer = 0;
        region.dstSubresource.layerCount = 1;
        region.dstSubresource.mipLevel = 0;
        region.srcOffset = VkOffset3D{ 0, 0, 0 };
        region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.srcSubresource.baseArrayLayer = 0;
        region.srcSubresource.layerCount = 1;
        region.srcSubresource.mipLevel = 0;

        const VkCommandBuffer cmdBuff = commandBuffer->GetHandle();
        const VkImage srcImage = renderTarget->GetColorImage(frameIndex);
        const VkImage dstImage = m_Images[frameIndex];
        vkCmdResolveImage(cmdBuff, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VkImageMemoryBarrier presentBarrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
        presentBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        presentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        presentBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        presentBarrier.dstAccessMask = VK_ACCESS_NONE;
        presentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        presentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        presentBarrier.image = m_Images[frameIndex];
        presentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        presentBarrier.subresourceRange.baseMipLevel = 0;
        presentBarrier.subresourceRange.levelCount = 1;
        presentBarrier.subresourceRange.baseArrayLayer = 0;
        presentBarrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            commandBuffer->GetHandle(),
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0, 0, nullptr, 0, nullptr, 1, &presentBarrier
        );
    }

    VkSwapchainKHR Swapchain::GetHandle() const
    {
        return m_Handle;
    }

    const VkSwapchainKHR* Swapchain::GetHandlePtr() const
    {
        return &m_Handle;
    }

    VkImage Swapchain::GetImage(uint32_t index) const
    {
        return m_Images[index];
    }

    VkImageView Swapchain::GetImageView(uint32_t index) const
    {
        return m_ImageViews[index];
    }
}
