#pragma once
#include <cstdint>

#include "Runtime/Object.h"

namespace Nova { class Window; }

namespace Nova::Rendering
{
    class Device;

    struct SurfaceCreateInfo
    {
        Device* device = nullptr;
        Window* window = nullptr;
    };

    class Surface : public Object
    {
    public:
        Surface() : Object("Surface") {}
        ~Surface() override = default;

        virtual bool Initialize(const SurfaceCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        Device* GetDevice() const;
        Window* GetWindow() const;

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        bool IsAvailable() const;
    protected:
        Device* m_Device = nullptr;
        Window* m_Window = nullptr;
    };
}
