#pragma once
#include "Containers/String.h"
#include "SwpchainBuffering.h"
#include "Filter.h"
#include <cstdint>

#include "Containers/StringView.h"


namespace Nova
{
    class Window;
}

namespace Nova::Rendering
{
    enum class DeviceType
    {
        Unknown,
        Vulkan
    };

    struct DeviceCreateInfo
    {
        String applicationName;
        uint16_t versionMajor, versionMinor;
        Window* window = nullptr;
        SwapchainBuffering buffering = SwapchainBuffering::DoubleBuffering;
        bool vSync = false;
    };

    class RenderTarget;
    class Texture;

    class Device
    {
    public:
        Device() = default;
        virtual ~Device() = default;

        static Device* Create(DeviceType type, const DeviceCreateInfo& createInfo);

        virtual bool Initialize(const DeviceCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;
        virtual void WaitIdle() = 0;
        virtual void SetName(StringView name) = 0;
    };
}
