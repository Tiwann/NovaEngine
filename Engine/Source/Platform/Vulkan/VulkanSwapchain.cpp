#include "VulkanSwapchain.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderer.h"
#include "VulkanRenderTarget.h"
#include "VulkanSemaphore.h"
#include "Containers/StringFormat.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"

namespace Nova
{
    bool VulkanSwapchain::Initialize(const SwapchainCreateInfo& CreateInfo)
    {
        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const VkSurfaceKHR Surface = Renderer->GetSurface();
        const VkDevice Device = Renderer->GetDevice();
        const u32 GraphicsQueueFamily = Renderer->GetGraphicsQueueFamily();
        const u32 PresentQueueFamily = Renderer->GetPresentQueueFamily();
        const auto& Convertor = Renderer->Convertor;

        VkSwapchainCreateInfoKHR SwapchainCreateInfo = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
        SwapchainCreateInfo.surface = Surface;
        SwapchainCreateInfo.imageFormat = Convertor.ConvertFormat(CreateInfo.ImageFormat);
        SwapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        SwapchainCreateInfo.presentMode = Convertor.ConvertPresentMode(CreateInfo.ImagePresentMode);
        SwapchainCreateInfo.clipped = true;
        SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        SwapchainCreateInfo.imageExtent.width = CreateInfo.ImageWidth;
        SwapchainCreateInfo.imageExtent.height = CreateInfo.ImageHeight;
        SwapchainCreateInfo.minImageCount = CreateInfo.ImageCount;
        SwapchainCreateInfo.imageArrayLayers = 1;
        SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        SwapchainCreateInfo.oldSwapchain = CreateInfo.Recycle ? m_Handle : nullptr;

        if (GraphicsQueueFamily == PresentQueueFamily)
        {
            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            SwapchainCreateInfo.queueFamilyIndexCount = 1;
            SwapchainCreateInfo.pQueueFamilyIndices = &GraphicsQueueFamily;
        }
        else
        {
            const u32 QueueFamilyIndices[2] = {GraphicsQueueFamily, PresentQueueFamily};
            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            SwapchainCreateInfo.queueFamilyIndexCount = 2;
            SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
        }

        VkSwapchainKHR NewSwapchain = nullptr;
        if (VK_FAILED(vkCreateSwapchainKHR(Device, &SwapchainCreateInfo, nullptr, &NewSwapchain)))
            return false;

        vkDestroySwapchainKHR(Device, m_Handle, nullptr);
        m_Handle = NewSwapchain;

        m_ImageCount = CreateInfo.ImageCount;
        m_ImageWidth = CreateInfo.ImageWidth;
        m_ImageHeight = CreateInfo.ImageHeight;
        m_ImageFormat = CreateInfo.ImageFormat;
        m_ImagePresentMode = CreateInfo.ImagePresentMode;

        if (VK_FAILED(vkGetSwapchainImagesKHR(Device, m_Handle, &m_ImageCount, m_Images)))
            return false;

        for (size_t i = 0; i < m_ImageCount; i++)
        {
            vkDestroyImageView(Device, m_ImageViews[i], nullptr);
            VkImageViewCreateInfo ImageViewCreateInfo = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
            ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ImageViewCreateInfo.format = Renderer->Convertor.ConvertFormat(m_ImageFormat);
            ImageViewCreateInfo.image = m_Images[i];
            ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ImageViewCreateInfo.subresourceRange.layerCount = 1;
            ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
            ImageViewCreateInfo.subresourceRange.levelCount = 1;
            ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;

            vkCreateImageView(Device, &ImageViewCreateInfo, nullptr, &m_ImageViews[i]);

            String ImageName = StringFormat("Swapchain Image {}", i);
            String ImageViewName = StringFormat("Swapchain Image View {}", i);
            VkDebugUtilsObjectNameInfoEXT ImageNameInfo{VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
            ImageNameInfo.objectType = VK_OBJECT_TYPE_IMAGE;
            ImageNameInfo.objectHandle = (u64)m_Images[i];
            ImageNameInfo.pObjectName = *ImageName;
            Renderer->GetFunctionPointers().vkSetDebugUtilsObjectNameEXT(Device, &ImageNameInfo);

            VkDebugUtilsObjectNameInfoEXT ImageViewNameInfo{VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
            ImageViewNameInfo.objectType = VK_OBJECT_TYPE_IMAGE_VIEW;
            ImageViewNameInfo.objectHandle = (u64)m_ImageViews[i];
            ImageViewNameInfo.pObjectName = *ImageViewName;
            Renderer->GetFunctionPointers().vkSetDebugUtilsObjectNameEXT(Device, &ImageViewNameInfo);
        }
        return true;
    }

    void VulkanSwapchain::SetDebugName(const String& Name)
    {
        if constexpr (!RendererIsDebug)
            return;
        VkDebugUtilsObjectNameInfoEXT NameInfo{VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT};
        NameInfo.objectType = VK_OBJECT_TYPE_SWAPCHAIN_KHR;
        NameInfo.objectHandle = (u64)m_Handle;
        NameInfo.pObjectName = *Name;

        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        Renderer->GetFunctionPointers().vkSetDebugUtilsObjectNameEXT(Device, &NameInfo);
    }

    bool VulkanSwapchain::Recreate()
    {
        const Application* Application = m_Owner->GetOwner();
        const Window* Window = Application->GetWindow();
        const u32 NewWidth = Window->GetWidth();
        const u32 NewHeight = Window->GetHeight();

        SwapchainCreateInfo CreateInfo;
        CreateInfo.ImageWidth = NewWidth;
        CreateInfo.ImageHeight = NewHeight;
        CreateInfo.ImageFormat = m_ImageFormat;
        CreateInfo.ImageCount = m_ImageCount;
        CreateInfo.ImagePresentMode = m_ImagePresentMode;
        CreateInfo.Recycle = true;
        return Initialize(CreateInfo);
    }

    void VulkanSwapchain::Destroy()
    {
        const VkDevice Device = GetOwner()->As<VulkanRenderer>()->GetDevice();
        for (size_t i = 0; i < m_ImageCount; i++)
        {
            vkDestroyImageView(Device, m_ImageViews[i], nullptr);
        }
        vkDestroySwapchainKHR(Device, m_Handle, nullptr);
    }

    VkImage VulkanSwapchain::GetImage(const size_t FrameIndex) const
    {
        return m_Images[FrameIndex];
    }

    void VulkanSwapchain::Resolve(const VulkanCommandBuffer* CommandBuffer, const VulkanRenderTarget* RenderTarget) const
    {
        const u32 FrameIndex = m_Owner->As<VulkanRenderer>()->GetCurrentFrameIndex();
        {
            VkImageMemoryBarrier ColorBarrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
            ColorBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            ColorBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            ColorBarrier.srcAccessMask = 0;
            ColorBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            ColorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            ColorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            ColorBarrier.image = m_Images[FrameIndex];
            ColorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ColorBarrier.subresourceRange.baseMipLevel = 0;
            ColorBarrier.subresourceRange.levelCount = 1;
            ColorBarrier.subresourceRange.baseArrayLayer = 0;
            ColorBarrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(
                CommandBuffer->GetHandle(),
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0, 0, nullptr, 0, nullptr, 1, &ColorBarrier
            );
        }


        VkImageResolve Region;
        Region.extent.width = m_ImageWidth;
        Region.extent.height = m_ImageHeight;
        Region.extent.depth = 1;
        Region.dstOffset = VkOffset3D{0, 0, 0};
        Region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Region.dstSubresource.baseArrayLayer = 0;
        Region.dstSubresource.layerCount = 1;
        Region.dstSubresource.mipLevel = 0;
        Region.srcOffset = VkOffset3D{0, 0, 0};
        Region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        Region.srcSubresource.baseArrayLayer = 0;
        Region.srcSubresource.layerCount = 1;
        Region.srcSubresource.mipLevel = 0;
        vkCmdResolveImage(CommandBuffer->GetHandle(), RenderTarget->GetColorImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                          m_Images[FrameIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);


        {
            VkImageMemoryBarrier ColorBarrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
            ColorBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            ColorBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            ColorBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            ColorBarrier.dstAccessMask = VK_ACCESS_NONE;
            ColorBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            ColorBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            ColorBarrier.image = m_Images[FrameIndex];
            ColorBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            ColorBarrier.subresourceRange.baseMipLevel = 0;
            ColorBarrier.subresourceRange.levelCount = 1;
            ColorBarrier.subresourceRange.baseArrayLayer = 0;
            ColorBarrier.subresourceRange.layerCount = 1;

            vkCmdPipelineBarrier(
                CommandBuffer->GetHandle(),
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0, 0, nullptr, 0, nullptr, 1, &ColorBarrier
            );
        }
    }

    u32 VulkanSwapchain::AcquireNextImage(const VulkanSemaphore* Semaphore) const
    {
        u32 NextFrameIndex = 0;
        const VkDevice Device = m_Owner->As<VulkanRenderer>()->GetDevice();
        if (VK_FAILED(vkAcquireNextImageKHR(Device, m_Handle, U64_MAX, Semaphore->GetHandle(), nullptr, &NextFrameIndex)))
        {
            return U32_MAX;
        }
        return NextFrameIndex;
    }
}
