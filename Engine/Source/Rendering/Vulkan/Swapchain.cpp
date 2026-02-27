#include "Swapchain.h"
#include "RenderDevice.h"
#include "Conversions.h"
#include "Containers/StringFormat.h"
#include "CommandBuffer.h"
#include "RenderTarget.h"
#include "VulkanExtensions.h"
#include "Rendering/Scoped.h"
#include "Sampler.h"

#include <vulkan/vulkan.h>

#include "Utils/VulkanUtils.h"

namespace Nova::Vulkan
{
    bool Swapchain::Initialize(const SwapchainCreateInfo& createInfo)
    {
        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        const Surface* surface = static_cast<const Surface*>(createInfo.surface);
        const VkSurfaceKHR surfaceHandle = surface->GetHandle();
        const VkDevice deviceHandle = device->GetHandle();
        const Queue* graphicsQueue = device->GetGraphicsQueue();
        const Queue* presentQueue = device->GetPresentQueue();
        const uint32_t graphicsQueueFamily = graphicsQueue->GetIndex();
        const uint32_t presentQueueFamily = presentQueue->GetIndex();

        VkSwapchainCreateInfoKHR swapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        swapchainCreateInfo.surface = surfaceHandle;
        swapchainCreateInfo.imageFormat = Convert<VkFormat>(createInfo.format);
        swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        swapchainCreateInfo.presentMode = Convert<VkPresentModeKHR>(createInfo.presentMode);
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
            ImageViewCreateInfo.format = Convert<VkFormat>(m_ImageFormat);
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

        CommandPool* commandPool = ((RenderDevice*)m_Device)->GetCommandPool();
        CommandBuffer commandBuffer = commandPool->AllocateCommandBuffer(CommandBufferLevel::Primary);

        commandBuffer.Begin({ CommandBufferUsageFlagBits::OneTimeSubmit });

        VkDependencyInfo inDependency = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
        inDependency.dependencyFlags = 0;
        inDependency.imageMemoryBarrierCount = barriers.Count();
        inDependency.pImageMemoryBarriers = barriers.Data();
        vkCmdPipelineBarrier2(commandBuffer.GetHandle(), &inDependency);

        commandBuffer.End();

        Scoped<Fence> fence = FenceCreateInfo(m_Device);
        graphicsQueue->Submit(&commandBuffer, nullptr, nullptr, &fence);
        fence->Wait(~0);

        commandBuffer.Free();
        SetName("Main Swapchain");
        m_Valid = true;
        return true;
    }

    void Swapchain::Destroy()
    {
        const RenderDevice* device = (RenderDevice*)m_Device;
        const VkDevice deviceHandle = device->GetHandle();

        for (size_t i = 0; i < GetImageCount(); i++)
            vkDestroyImageView(deviceHandle, m_ImageViews[i], nullptr);

        vkDestroySwapchainKHR(deviceHandle, m_Handle, nullptr);
    }

    bool Swapchain::Recreate()
    {
        const Nova::Surface* surface = GetSurface();
        SwapchainCreateInfo createInfo;
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
        SetVkObjectDebugName(m_Device, VK_OBJECT_TYPE_SWAPCHAIN_KHR, m_Handle, name);
    }

    bool Swapchain::AcquireNextImage(const Semaphore* semaphore, const Fence* fence, uint32_t& frameIndex) const
    {
        if (!semaphore)
            return false;

        const RenderDevice* device = (RenderDevice*)m_Device;
        const VkDevice deviceHandle = device->GetHandle();
        const VkResult result = vkAcquireNextImageKHR(deviceHandle, m_Handle, 1000000000, semaphore->GetHandle(), fence ? fence->GetHandle() : nullptr, &frameIndex);
        if (result != VK_SUCCESS)
            return false;

        return true;
    }

    Ref<Nova::Texture> Swapchain::GetTexture(const uint32_t index)
    {
        const auto createTexture = [this, &index]() -> Ref<Texture>
        {
            Texture texture;
            texture.m_Device = (RenderDevice*)m_Device;
            texture.m_Image = m_Images[index];
            texture.m_ImageView = m_ImageViews[index];
            texture.m_Width = m_ImageWidth;
            texture.m_Height = m_ImageHeight;
            texture.m_Depth = 1;
            texture.m_UsageFlags = TextureUsageFlagBits::None;
            texture.m_Name = StringFormat("Swapchain Texture {}", index);
            texture.m_Allocation = nullptr;
            texture.m_SampleCount = 1;
            texture.m_Mips = 1;
            texture.m_State = ResourceState::ColorAttachment;
            texture.m_Format = m_ImageFormat;
            return MakeRef<Texture>(texture);
        };

        return m_Textures[index].Get(createTexture);
    }

    Ref<Nova::Texture> Swapchain::GetCurrentTexture()
    {
        const RenderDevice* device = (RenderDevice*)m_Device;
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
