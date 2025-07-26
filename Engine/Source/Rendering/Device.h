#pragma once
#include "Containers/String.h"
#include "Filter.h"
#include <cstdint>

namespace Nova
{
    class Window;
}

namespace Nova::Rendering
{
    struct DeviceCreateInfo
    {
        String applicationName;
        uint16_t versionMajor, versionMinor;
        Window* window;
    };

    class RenderTarget;
    class Texture;

    class Device
    {
    public:
        Device() = default;
        virtual ~Device() = default;

        virtual bool Initialize(const DeviceCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;
        virtual void WaitIdle() = 0;
        virtual void ResolveToSwapchain(RenderTarget& renderTarget) = 0;
        virtual void BlitToSwapchain(RenderTarget& renderTarget, Filter filter) = 0;
        virtual void BlitToSwapchain(Texture& texture, Filter filter) = 0;
        virtual void BlitToRenderTarget(Texture& srcTexture, RenderTarget& destRenderTarget, uint32_t x, uint32_t y) = 0;
    };
}
