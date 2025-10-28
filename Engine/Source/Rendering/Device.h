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
        String appName;
        Window* window = nullptr;
        SwapchainBuffering buffering = SwapchainBuffering::DoubleBuffering;
        bool vSync = false;
    };
    
    class Surface;
    struct SurfaceCreateInfo;
    class RenderTarget;
    class Texture;
    struct TextureCreateInfo;
    class Sampler;
    struct SamplerCreateInfo;
    class Buffer;
    struct BufferCreateInfo;
    class GraphicsPipeline;
    struct GraphicsPipelineCreateInfo;
    class ComputePipeline;
    struct ComputePipelineCreateInfo;
    class Shader;
    struct ShaderCreateInfo;

    class Device : public Object
    {
    public:
        Device() : Object("Rendering Device")
        {
        }

        ~Device() override = default;

        virtual bool Initialize(const DeviceCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;
        virtual void WaitIdle() const = 0;
        virtual void SetName(StringView name) = 0;
        virtual DeviceType GetDeviceType() = 0;

        virtual Ref<Surface> CreateSurface(const SurfaceCreateInfo& createInfo) = 0;
        virtual Ref<Texture> CreateTexture(const TextureCreateInfo& createInfo) = 0;
        virtual Ref<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) = 0;
        virtual Ref<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) = 0;
        virtual Ref<Shader> CreateShader(const ShaderCreateInfo& createInfo) = 0;
        virtual Ref<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) = 0;
        virtual Ref<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) = 0;

        virtual uint32_t GetImageCount() const = 0;

        StringView GetDeviceVendor() const;
    protected:
        String m_DeviceVendor;
    };

    Ref<Device> CreateRenderDevice(DeviceType type, const DeviceCreateInfo& createInfo);
}
