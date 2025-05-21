#pragma once
#include "RendererObject.h"
#include "PresentMode.h"
#include "Runtime/Format.h"

namespace Nova
{
    class Renderer;

    struct SwapchainCreateInfo
    {
        Format ImageFormat;
        uint32_t ImageCount;
        PresentMode ImagePresentMode;
        u32 ImageWidth, ImageHeight;
        bool Recycle;
    };

    class Swapchain : public RendererObject<SwapchainCreateInfo>
    {
    public:
        explicit Swapchain(Renderer* Owner) : RendererObject("Swapchain", Owner){}
        ~Swapchain() override = default;

        virtual bool Recreate() = 0;

        i32 GetWidth() const { return m_ImageWidth; }
        i32 GetHeight() const { return m_ImageHeight; }
        Format GetFormat() const { return m_ImageFormat; }
        PresentMode GetPresentMode() const { return m_ImagePresentMode; }
        i32 GetImageCount() const { return m_ImageCount; }
    protected:
        Format m_ImageFormat = Format::None;
        u32 m_ImageCount = 0;
        PresentMode m_ImagePresentMode = PresentMode::Unknown;
        u32 m_ImageWidth = 0, m_ImageHeight = 0;
    };
}
