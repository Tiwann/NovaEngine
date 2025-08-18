﻿#include "Swapchain.h"
#include "Device.h"
#include "Conversions.h"
#include "Containers/StringFormat.h"
#include "CommandBuffer.h"
#include "RenderTarget.h"
#include "VulkanExtensions.h"
#include "Rendering/Scoped.h"

#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool Swapchain::Initialize(const Rendering::SwapchainCreateInfo& createInfo)
    {
        Device* device = static_cast<Device*>(createInfo.device);
        const Surface* surface = static_cast<const Surface*>(createInfo.surface);
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

        Array<VkImageMemoryBarrier2> barriers;

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

            m_Textures[i].SetDirty();

            VkImageMemoryBarrier2 barrier { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
            barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
            barrier.srcAccessMask = VK_ACCESS_2_NONE;
            barrier.dstAccessMask = VK_ACCESS_2_NONE;
            barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier.image = m_Images[i];
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            barrier.subresourceRange.baseMipLevel = 0;
            barrier.subresourceRange.levelCount = 1;
            barrier.subresourceRange.baseArrayLayer = 0;
            barrier.subresourceRange.layerCount = 1;
            barrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
            barrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
            barriers.Add(barrier);
        }

        CommandPool* commandPool = ((Device*)m_Device)->GetCommandPool();
        CommandBuffer commandBuffer = commandPool->AllocateCommandBuffer(Rendering::CommandBufferLevel::Primary);

        commandBuffer.Begin({ Rendering::CommandBufferUsageFlagBits::OneTimeSubmit });

        VkDependencyInfo inDependency = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        inDependency.dependencyFlags = 0;
        inDependency.imageMemoryBarrierCount = barriers.Count();
        inDependency.pImageMemoryBarriers = barriers.Data();
        vkCmdPipelineBarrier2(commandBuffer.GetHandle(), &inDependency);

        commandBuffer.End();

        Rendering::Scoped<Fence> fence = Rendering::FenceCreateInfo(m_Device);
        graphicsQueue->Submit(&commandBuffer, nullptr, nullptr, &fence);
        fence->Wait(~0);

        commandBuffer.Free();

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        if (!m_Name.IsEmpty())
        {
            SetName(m_Name);
        }
#endif
        m_Valid = true;
        return true;
    }

    void Swapchain::Destroy()
    {
        const Device* device = (Device*)m_Device;
        const VkDevice deviceHandle = device->GetHandle();

        for (size_t i = 0; i < GetImageCount(); i++)
            vkDestroyImageView(deviceHandle, m_ImageViews[i], nullptr);

        vkDestroySwapchainKHR(deviceHandle, m_Handle, nullptr);
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

    void Swapchain::SetName(StringView name)
    {
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        const VkDevice deviceHandle = ((Device*)m_Device)->GetHandle();
        VkDebugUtilsObjectNameInfoEXT info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        info.objectHandle = (uint64_t)m_Handle;
        info.objectType = VK_OBJECT_TYPE_SWAPCHAIN_KHR;
        info.pObjectName = *name;
        vkSetDebugUtilsObjectName(deviceHandle, &info);
#endif
    }

    bool Swapchain::AcquireNextImage(const Semaphore* semaphore, const Fence* fence, uint32_t& frameIndex)
    {
        if (!semaphore)
            return false;

        const Device* device = (Device*)m_Device;
        const VkDevice deviceHandle = device->GetHandle();
        const VkResult result = vkAcquireNextImageKHR(deviceHandle, m_Handle, 1000000000, semaphore->GetHandle(), fence ? fence->GetHandle() : nullptr, &frameIndex);
        if (result != VK_SUCCESS)
            return false;

        return true;
    }

    const Ref<Texture>& Swapchain::GetTexture(const uint32_t index)
    {
        const auto createTexture = [this, &index]() -> Ref<Texture>
        {
            Ref<Texture> texture = new Texture();
            texture->m_Device = (Device*)m_Device;
            texture->m_Image = m_Images[index];
            texture->m_ImageView = m_ImageViews[index];
            texture->m_Width = m_ImageWidth;
            texture->m_Height = m_ImageHeight;
            texture->m_Allocation = nullptr;
            texture->m_SampleCount = 1;
            texture->m_Mips = 1;
            texture->m_UsageFlags = Rendering::TextureUsageFlagBits::Attachment;
            texture->m_Format = m_ImageFormat;
            return texture;
        };

        return m_Textures[index].Get(createTexture);
    }

    const Ref<Texture>& Swapchain::GetCurrentTexture()
    {
        const Device* device = (Device*)m_Device;
        const size_t imageIndex = device->GetCurrentFrameIndex();
        return GetTexture(imageIndex);
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
