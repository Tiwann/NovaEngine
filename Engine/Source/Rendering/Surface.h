#pragma once
#include <cstdint>
#include "Runtime/Object.h"

namespace Nova
{
    class Window;
    class RenderDevice;

    struct SurfaceCreateInfo
    {
        RenderDevice* device = nullptr;
        Window* window = nullptr;

        SurfaceCreateInfo& withDevice(RenderDevice* inDevice) { device = inDevice; return *this; };
        SurfaceCreateInfo& withWindow(Window* inWindow) { window = inWindow; return *this; };
    };

    class Surface : public Object
    {
    public:
        Surface() : Object("Surface") {}
        ~Surface() override = default;

        virtual bool Initialize(const SurfaceCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        RenderDevice* GetDevice() const;
        Window* GetWindow() const;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        bool IsAvailable() const;
        void SwapBuffers() const;
    protected:
        RenderDevice* m_Device = nullptr;
        Window* m_Window = nullptr;
    };
}
