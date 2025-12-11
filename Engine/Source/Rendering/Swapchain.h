#pragma once
#include "Runtime/Format.h"
#include "PresentMode.h"
#include "SwpchainBuffering.h"
#include "Containers/StringView.h"

#include <cstdint>


namespace Nova
{
    class Device;
    class Surface;

    struct SwapchainCreateInfo
    {
        Device* device = nullptr;
        Surface* surface = nullptr;
        Format format = Format::None;
        SwapchainBuffering buffering = SwapchainBuffering::None;
        PresentMode presentMode = PresentMode::Unknown;
        uint32_t width = 0;
        uint32_t height = 0;
        bool recycle = false;
    };

    class Swapchain
    {
    public:
        Swapchain() = default;
        virtual ~Swapchain() = default;

        virtual bool Initialize(const SwapchainCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        virtual bool Recreate() = 0;
        virtual void SetName(StringView name) = 0;


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
        bool m_HasVSync = false;
        bool m_Valid = true;
    };
}
