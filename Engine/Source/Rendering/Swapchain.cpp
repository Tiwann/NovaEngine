#include "Swapchain.h"

namespace Nova
{
    i32 Swapchain::GetWidth() const
    {
        return m_ImageWidth;
    }

    i32 Swapchain::GetHeight() const
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

    u32 Swapchain::GetImageCount() const
    {
        return (u32)m_Buffering;
    }
}
