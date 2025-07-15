#pragma once
#include "Runtime/Format.h"
#include "PresentMode.h"
#include "SwpchainBuffering.h"

#include <cstdint>

namespace Nova::Rendering
{
    class Device;
    class Surface;

    struct SwapchainCreateInfo
    {
        Device* device;
        Surface* surface;
        Format format;
        SwapchainBuffering buffering;
        PresentMode presentMode;
        uint32_t width;
        uint32_t height;
        bool recycle;
    };

    class Swapchain
    {
    public:
        Swapchain() = default;
        virtual ~Swapchain() = default;

        virtual bool Initialize(const SwapchainCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual bool Recreate() = 0;


        int32_t GetWidth() const;
        int32_t GetHeight() const;
        Format GetFormat() const;
        PresentMode GetPresentMode() const;
        uint32_t GetImageCount() const;

        Device* GetDevice() const;
        Surface* GetSurface() const;

        void Invalidate();

        bool IsValid() const;

    protected:
        Device* m_Device = nullptr;
        Surface* m_Surface = nullptr;
        Format m_ImageFormat = Format::None;
        SwapchainBuffering m_Buffering = SwapchainBuffering::None;
        PresentMode m_ImagePresentMode = PresentMode::Unknown;
        uint32_t m_ImageWidth = 0, m_ImageHeight = 0;
        bool m_Valid = true;
    };
}
