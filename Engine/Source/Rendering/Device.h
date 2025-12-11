#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "SwpchainBuffering.h"
#include "Runtime/Ref.h"
#include "Runtime/Object.h"
#include "Material.h"
#include <cstdint>

#include "Buffer.h"
#include "Texture.h"

#define NOVA_MAX_IMAGE_COUNT 3

namespace Nova
{
    class Window;
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
    class Material;
    struct MaterialCreateInfo;
    class Fence;
    struct FenceCreateInfo;

    enum class DeviceType
    {
        Null,
        Vulkan,
#ifdef NOVA_HAS_D3D12
        D3D12
#endif
    };

    struct DeviceCreateInfo
    {
        String appName;
        Window* window = nullptr;
        SwapchainBuffering buffering = SwapchainBuffering::DoubleBuffering;
        bool vSync = false;
    };

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
        virtual Ref<Texture> CreateTextureUnitialized() = 0;
        virtual Ref<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) = 0;
        virtual Ref<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) = 0;
        virtual Ref<Shader> CreateShader(const ShaderCreateInfo& createInfo) = 0;
        virtual Ref<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) = 0;
        virtual Ref<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) = 0;
        virtual Ref<Material> CreateMaterial(const MaterialCreateInfo& createInfo) = 0;
        virtual Ref<Fence> CreateFence(const FenceCreateInfo& createInfo) = 0;


        Ref<Fence> CreateFence();
        Ref<Buffer> CreateBuffer(BufferUsage usage, size_t size);
        Ref<Texture> CreateTexture(TextureUsageFlags usageFlags, uint32_t width, uint32_t height, Format format);
        Ref<Material> CreateMaterial(Ref<Shader> material);
        Ref<ComputePipeline> CreateComputePipeline(Ref<Shader> shader);
        Ref<Sampler> CreateSampler();

        virtual uint32_t GetImageCount() const = 0;

        StringView GetDeviceVendor() const;
        bool HasVSync() const;
    protected:
        String m_DeviceVendor;
        bool m_VSync;
    };

    Ref<Device> CreateRenderDevice(DeviceType type, const DeviceCreateInfo& createInfo);
}
