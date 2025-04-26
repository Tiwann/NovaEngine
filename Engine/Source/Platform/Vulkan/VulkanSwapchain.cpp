#include "VulkanSwapchain.h"
#include "VulkanRenderer.h"
#include "Runtime/Log.h"

namespace Nova
{
    bool VulkanSwapchain::Initialize(const SwapchainDescription& Description)
    {
        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const auto& Convertor = Renderer->Convertor;

        VkSwapchainCreateInfoKHR SwapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        //SwapchainCreateInfo.surface = m_Surface;
        SwapchainCreateInfo.imageFormat = Convertor.ConvertFormat(Description.ImageFormat);
        //SwapchainCreateInfo.imageColorSpace = m_ImageColorSpace = SurfaceFormats[0].colorSpace;
        SwapchainCreateInfo.presentMode = Convertor.ConvertPresentMode(Description.ImagePresentMode);
        SwapchainCreateInfo.clipped = true;
        SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        SwapchainCreateInfo.imageExtent.width = Description.ImageWidth;
        SwapchainCreateInfo.imageExtent.height = Description.ImageHeight;
        SwapchainCreateInfo.minImageCount = Description.ImageCount;
        SwapchainCreateInfo.imageArrayLayers = 1;
        SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        SwapchainCreateInfo.oldSwapchain = nullptr;

        const VkDevice Device = Renderer->GetDevice();
        if (VK_FAILED(vkCreateSwapchainKHR(Device, &SwapchainCreateInfo, nullptr, &m_Handle)))
        {
            NOVA_VULKAN_ERROR("Failed to create swapchain!");
            return false;
        }

        return true;
    }

    bool VulkanSwapchain::Resize(const i32 Width, const i32 Height)
    {
        Destroy();

        const VulkanRenderer* Renderer = GetOwner()->As<VulkanRenderer>();
        const auto& Convertor = Renderer->Convertor;

        VkSwapchainCreateInfoKHR SwapchainCreateInfo = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
        //SwapchainCreateInfo.surface = m_Surface;
        SwapchainCreateInfo.imageFormat = Convertor.ConvertFormat(m_ImageFormat);
        //SwapchainCreateInfo.imageColorSpace = m_ImageColorSpace = SurfaceFormats[0].colorSpace;
        SwapchainCreateInfo.presentMode = Convertor.ConvertPresentMode(m_ImagePresentMode);
        SwapchainCreateInfo.clipped = true;
        SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        SwapchainCreateInfo.imageExtent.width = m_ImageWidth;
        SwapchainCreateInfo.imageExtent.height = m_ImageHeight;
        SwapchainCreateInfo.minImageCount = m_ImageCount;
        SwapchainCreateInfo.imageArrayLayers = 1;
        SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        SwapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        SwapchainCreateInfo.oldSwapchain = nullptr;

        const VkDevice Device = Renderer->GetDevice();
        if (VK_FAILED(vkCreateSwapchainKHR(Device, &SwapchainCreateInfo, nullptr, &m_Handle)))
        {
            NOVA_VULKAN_ERROR("Failed to create swapchain!");
            return false;
        }
        return false;
    }

    void VulkanSwapchain::Destroy()
    {
        const VkDevice Device = GetOwner()->As<VulkanRenderer>()->GetDevice();
        vkDestroySwapchainKHR(Device, m_Handle, nullptr);
    }
}
