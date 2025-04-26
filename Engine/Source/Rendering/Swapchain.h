#pragma once
#include "PresentMode.h"
#include "Runtime/Format.h"
#include "Runtime/Object.h"

namespace Nova
{
    class Renderer;

    struct SwapchainDescription
    {
        Format ImageFormat;
        uint32_t ImageCount;
        PresentMode ImagePresentMode;
        i32 ImageWidth, ImageHeight;
        class Swapchain* OldSwapchain;
    };

    class Swapchain : public Object
    {
    public:
        explicit Swapchain(Renderer* Owner) : Object("Swapchain"), m_Owner(Owner) {}
        ~Swapchain() override = default;

        virtual bool Initialize(const SwapchainDescription& Description) = 0;
        virtual bool Resize(const i32 Width, const i32 Height) = 0;
        virtual void Destroy() = 0;

        i32 GetWidth() const { return m_ImageWidth; }
        i32 GetHeight() const { return m_ImageHeight; }
        Format GetFormat() const { return m_ImageFormat; }
        PresentMode GetPresentMode() const { return m_ImagePresentMode; }
        i32 GetImageCount() const { return m_ImageCount; }
        Renderer* GetOwner() const { return m_Owner; }
    protected:
        i32 m_ImageWidth, m_ImageHeight;
        Format m_ImageFormat;
        i32 m_ImageCount;
        PresentMode m_ImagePresentMode;
    private:
        Renderer* m_Owner = nullptr;
    };
}
