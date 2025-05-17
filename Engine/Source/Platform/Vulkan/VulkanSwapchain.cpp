#include "VulkanSwapchain.h"
#include "VulkanRenderer.h"
#include "Containers/StaticArray.h"
#include "Runtime/Application.h"
#include "Runtime/Log.h"
#include "Runtime/Window.h"

namespace Nova
{
    bool VulkanSwapchain::Initialize(const SwapchainCreateInfo& CreateInfo)
    {
        Swapchain::Initialize(CreateInfo);
        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const VkSurfaceKHR Surface = Renderer->GetSurface();
        const VkPhysicalDevice PhysicalDevice = Renderer->GetPhysicalDevice();
        const u32 GraphicsQueueIndex = Renderer->GetGraphicsQueueFamily();
        const u32 PresentQueueIndex = Renderer->GetPresentQueueFamily();

        const auto& Convertor = Renderer->Convertor;

        u32 SurfaceFormatCount;
        VkSurfaceFormatKHR SurfaceFormats[32];
        vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &SurfaceFormatCount, SurfaceFormats);

        VkSwapchainCreateInfoKHR SwapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
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
        SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        SwapchainCreateInfo.oldSwapchain = CreateInfo.OldSwapchain ? CreateInfo.OldSwapchain->As<VulkanSwapchain>()->GetHandle() : nullptr;

        if (GraphicsQueueIndex == PresentQueueIndex)
        {
            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            SwapchainCreateInfo.queueFamilyIndexCount = 1;
            SwapchainCreateInfo.pQueueFamilyIndices = &GraphicsQueueIndex;
        }
        else
        {
            const u32 QueueFamilyIndices[2] = { GraphicsQueueIndex, PresentQueueIndex };
            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            SwapchainCreateInfo.queueFamilyIndexCount = std::size(QueueFamilyIndices);
            SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
        }

        const VkDevice Device = Renderer->GetDevice();
        if (CreateInfo.OldSwapchain)
        {
            VkSwapchainKHR NewSwapchain = nullptr;
            if (VK_FAILED(vkCreateSwapchainKHR(Device, &SwapchainCreateInfo, nullptr, &NewSwapchain)))
                return false;

            vkDestroySwapchainKHR(Device, m_Handle, nullptr);
            m_Handle = NewSwapchain;
        } else
        {
            if (VK_FAILED(vkCreateSwapchainKHR(Device, &SwapchainCreateInfo, nullptr, &m_Handle)))
                return false;
        }
        m_CachedCreateInfo = CreateInfo;
        return true;
    }

    bool VulkanSwapchain::Resize(const i32 Width, const i32 Height)
    {
        if (m_Width == Width && m_Height == Height)
            return true;
        SwapchainCreateInfo CreateInfo = m_CachedCreateInfo;
        CreateInfo.ImageWidth = Width;
        CreateInfo.ImageHeight = Height;
        CreateInfo.OldSwapchain = this;
        m_Width = Width;
        m_Height = Height;
        return Initialize(CreateInfo);
    }

    bool VulkanSwapchain::Recreate()
    {
        const Renderer* Renderer = GetOwner();
        const Application* Application = Renderer->GetOwner();
        const Window* Window = Application->GetWindow();

        SwapchainCreateInfo CreateInfo = m_CachedCreateInfo;
        CreateInfo.OldSwapchain = this;
        CreateInfo.ImageWidth = Window->GetWidth();
        CreateInfo.ImageHeight = Window->GetHeight();
        return Initialize(CreateInfo);
    }

    void VulkanSwapchain::Destroy()
    {
        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        vkDestroySwapchainKHR(Device, m_Handle, nullptr);
    }

    StaticArray<VkImage, 3> VulkanSwapchain::GetImages(u32& OutImageCount) const
    {
        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();

        StaticArray<VkImage, 3> Images;
        vkGetSwapchainImagesKHR(Device, m_Handle, &OutImageCount, Images.Data());
        return Images;
    }
}
