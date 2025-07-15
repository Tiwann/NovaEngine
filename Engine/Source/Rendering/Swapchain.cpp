#include "Swapchain.h"

#include "Surface.h"
#include "Runtime/DesktopWindow.h"
#include "Runtime/Window.h"

namespace Nova::Rendering
{
    int32_t Swapchain::GetWidth() const
    {
        return m_ImageWidth;
    }

    int32_t Swapchain::GetHeight() const
    {
        return m_ImageHeight;
    }

    Format Swapchain::GetFormat() const
    {
        return m_ImageFormat;
    }

    PresentMode Swapchain::GetPresentMode() const
    {
        return m_ImagePresentMode;
    }

    uint32_t Swapchain::GetImageCount() const
    {
        return (uint32_t)m_Buffering;
    }

    Device* Swapchain::GetDevice() const
    {
        return m_Device;
    }

    Surface* Swapchain::GetSurface() const
    {
        return m_Surface;
    }

    void Swapchain::Invalidate()
    {
        m_Valid = false;
    }

    bool Swapchain::IsValid() const
    {
        const Window* window = m_Surface->GetWindow();
        const DesktopWindow* desktopWindow = dynamic_cast<const DesktopWindow*>(window);
        if (desktopWindow)
        {
            const bool minimized = desktopWindow->IsMinimized();
            return m_Valid && !minimized;
        } else
        {
            return m_Valid;
        }
    }
}
