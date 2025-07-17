#include "Surface.h"
#include "Device.h"
#include "Runtime/DesktopWindow.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace Nova::Vulkan
{
    bool Surface::Initialize(const Rendering::SurfaceCreateInfo& createInfo)
    {
        if (!createInfo.window)
            return false;

        m_Device = createInfo.device;
        m_Window = createInfo.window;
        const Device* device = static_cast<Device*>(createInfo.device);
        const VkInstance instance = device->GetInstance();

        if (DesktopWindow* window = static_cast<DesktopWindow*>(createInfo.window))
        {
            GLFWwindow* windowHandle = window->GetHandle();
            if (glfwCreateWindowSurface(instance, windowHandle, nullptr, &m_Handle) != VK_SUCCESS)
                return false;
            return true;
        }

        return false;
    }

    void Surface::Destroy()
    {
        const Device* device = static_cast<Device*>(m_Device);
        const VkInstance instance = device->GetInstance();
        vkDestroySurfaceKHR(instance, m_Handle, nullptr);
    }

    VkSurfaceKHR Surface::GetHandle() const
    {
        return m_Handle;
    }

    const VkSurfaceKHR* Surface::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
