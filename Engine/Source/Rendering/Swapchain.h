#pragma once
#include "RendererObject.h"
#include "PresentMode.h"
#include "Runtime/Format.h"
#include "Rendering/SwpchainBuffering.h"

namespace Nova
{
    class Renderer;

    struct SwapchainCreateInfo
    {
        Format ImageFormat;
        SwapchainBuffering Buffering;
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

        i32 GetWidth() const;
        i32 GetHeight() const;
        Format GetFormat() const;
        PresentMode GetPresentMode() const;
        u32 GetImageCount() const;

    protected:
        Format m_ImageFormat = Format::None;
        SwapchainBuffering m_Buffering = SwapchainBuffering::None;
        PresentMode m_ImagePresentMode = PresentMode::Unknown;
        u32 m_ImageWidth = 0, m_ImageHeight = 0;
    };
}
