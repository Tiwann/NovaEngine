#include "VulkanSurface.h"
#include "Runtime/DesktopWindow.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "VulkanRenderer.h"
#include "Rendering/Renderer.h"

namespace Nova
{
    bool VulkanSurface::Initialize(const SurfaceCreateInfo& CreateInfo)
    {
        if (!CreateInfo.Window)
            return false;

        const VkInstance Instance = m_Owner->As<VulkanRenderer>()->GetInstance();

        if (DesktopWindow* Window = CreateInfo.Window->As<DesktopWindow>())
        {
            GLFWwindow* WindowHandle = Window->GetHandle();
            if (VK_FAILED(glfwCreateWindowSurface(Instance, WindowHandle, nullptr, &m_Handle)))
                return false;
            return true;
        }

        return false;
    }

    void VulkanSurface::Destroy()
    {
        const VkInstance Instance = m_Owner->As<VulkanRenderer>()->GetInstance();
        vkDestroySurfaceKHR(Instance, m_Handle, nullptr);
    }

    void VulkanSurface::SetDebugName(const String& Name)
    {
        if constexpr(!RendererIsDebug)
            return;

        VkDebugUtilsObjectNameInfoEXT NameInfo { VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
        NameInfo.objectType = VK_OBJECT_TYPE_SURFACE_KHR;
        NameInfo.objectHandle = (u64)m_Handle;
        NameInfo.pObjectName = *Name;

        const VulkanRenderer* Renderer = m_Owner->As<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        Renderer->GetFunctionPointers().vkSetDebugUtilsObjectNameEXT(Device, &NameInfo);
    }

    VkSurfaceKHR VulkanSurface::GetHandle() const
    {
        return m_Handle;
    }

    const VkSurfaceKHR* VulkanSurface::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
