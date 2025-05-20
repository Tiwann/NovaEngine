#include "VulkanSwapchain.h"
#include "VulkanRenderer.h"
#include "Runtime/Application.h"
#include "Runtime/Log.h"
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
        SwapchainCreateInfo.oldSwapchain = CreateInfo.Recycle ? m_Handle : nullptr;

        if (GraphicsQueueFamily == PresentQueueFamily)
        {
            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            SwapchainCreateInfo.queueFamilyIndexCount = 1;
            SwapchainCreateInfo.pQueueFamilyIndices = &GraphicsQueueFamily;
        }
        else
        {
            const u32 QueueFamilyIndices[2] = { GraphicsQueueFamily, PresentQueueFamily };
            SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            SwapchainCreateInfo.queueFamilyIndexCount = 2;
            SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
        }

        if (CreateInfo.Recycle)
        {
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
            return true;
        }

        if (VK_FAILED(vkCreateSwapchainKHR(Device, &SwapchainCreateInfo, nullptr, &m_Handle)))
            return false;

        m_ImageCount = CreateInfo.ImageCount;
        m_ImageWidth = CreateInfo.ImageWidth;
        m_ImageHeight = CreateInfo.ImageHeight;
        m_ImageFormat = CreateInfo.ImageFormat;
        m_ImagePresentMode = CreateInfo.ImagePresentMode;
        return true;
    }

    void VulkanSwapchain::SetDebugName(const String& Name)
    {
        if constexpr(!RendererIsDebug)
            return;
        VkDebugUtilsObjectNameInfoEXT NameInfo { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
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
        vkDestroySwapchainKHR(Device, m_Handle, nullptr);
    }

    void VulkanSwapchain::GetImages(VkImage* OutImages)
    {
        const VkDevice Device = GetOwner()->As<VulkanRenderer>()->GetDevice();
        vkGetSwapchainImagesKHR(Device, m_Handle, &m_ImageCount, OutImages);
    }
}
