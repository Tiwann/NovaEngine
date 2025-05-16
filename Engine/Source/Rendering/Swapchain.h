#pragma once
#include "PresentMode.h"
#include "Runtime/Format.h"
#include "Runtime/Object.h"

namespace Nova
{
    class Renderer;

    struct SwapchainCreateInfo
    {
        Format ImageFormat;
        uint32_t ImageCount;
        PresentMode ImagePresentMode;
        i32 ImageWidth, ImageHeight;
        u32 GraphicsQueueIndex, PresentQueueIndex;
        class Swapchain* OldSwapchain;
    };

    class Swapchain : public Object
    {
    public:
        explicit Swapchain(Renderer* Owner) : Object("Swapchain"), m_Owner(Owner) {}
        ~Swapchain() override = default;

        virtual bool Initialize(const SwapchainCreateInfo& Description) = 0;
        virtual bool Resize(const i32 Width, const i32 Height) = 0;
        virtual bool Recreate() = 0;
        virtual void Destroy() = 0;

        i32 GetWidth() const { return m_Width; }
        i32 GetHeight() const { return m_Height; }
        Format GetFormat() const { return m_ImageFormat; }
        PresentMode GetPresentMode() const { return m_ImagePresentMode; }
        i32 GetImageCount() const { return m_ImageCount; }
        Renderer* GetOwner() const { return m_Owner; }
    protected:
        i32 m_Width = 0, m_Height = 0;
        Format m_ImageFormat = Format::NONE;
        i32 m_ImageCount = 0;
        PresentMode m_ImagePresentMode = PresentMode::Unknown;
    private:
        Renderer* m_Owner = nullptr;
    };
}
