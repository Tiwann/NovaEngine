#include "Swapchain.h"
#include "Surface.h"

namespace Nova
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
        return m_Valid;
    }
}
