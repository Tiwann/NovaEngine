#include "Swapchain.h"

namespace Nova
{
    bool Swapchain::Initialize(const SwapchainCreateInfo& CreateInfo)
    {
        m_Width = CreateInfo.ImageWidth;
        m_Height = CreateInfo.ImageHeight;
        m_ImageFormat = CreateInfo.ImageFormat;
        m_ImagePresentMode = CreateInfo.ImagePresentMode;
        m_ImageCount = CreateInfo.ImageCount;
        return true;
    }

    i32 Swapchain::GetWidth() const
    {
        return m_Width;
    }

    i32 Swapchain::GetHeight() const
    {
        return m_Height;
    }

    Format Swapchain::GetFormat() const
    {
        return m_ImageFormat;
    }

    PresentMode Swapchain::GetPresentMode() const
    {
        return m_ImagePresentMode;
    }

    i32 Swapchain::GetImageCount() const
    {
        return m_ImageCount;
    }

    Renderer* Swapchain::GetOwner() const
    {
        return m_Owner;
    }
}
