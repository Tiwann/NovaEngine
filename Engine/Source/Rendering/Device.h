#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "SwpchainBuffering.h"
#include "Runtime/Ref.h"
#include "Runtime/Object.h"
#include <cstdint>


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

    class Device : public Object
    {
    public:
        Device() : Object("Rendering Device") {}
        ~Device() override = default;

        virtual bool Initialize(const DeviceCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;
        virtual void WaitIdle() = 0;
        virtual void SetName(StringView name) = 0;
        virtual DeviceType GetDeviceType() = 0;
    };

    Ref<Device> CreateRenderDevice(DeviceType type, const DeviceCreateInfo& createInfo);
}
