#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/Map.h"
#include "SwpchainBuffering.h"
#include "Runtime/Ref.h"
#include "Runtime/Object.h"
#include "Material.h"
#include "Buffer.h"
#include "Texture.h"
#include "RenderDeviceType.h"
#include <cstdint>

#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(RenderDevice, "RENDER DEVICE")

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
    class RenderTarget;
    struct RenderTargetCreateInfo;
    class Swapchain;
    class CommandBuffer;


    struct DeviceCreateInfo
    {
        String appName;
        Window* window = nullptr;
        SwapchainBuffering buffering = SwapchainBuffering::DoubleBuffering;
        bool vSync = false;
    };

    class RenderDevice : public Object
    {
    public:
        RenderDevice();

        ~RenderDevice() override = default;

        virtual bool Initialize(const DeviceCreateInfo& createInfo) = 0;
        virtual void Destroy();

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;
        virtual void WaitIdle() const = 0;
        virtual void SetName(StringView name) = 0;

        virtual Nova::Surface* GetSurface() { return nullptr; }
        virtual Nova::Swapchain* GetSwapchain() { return nullptr; }
        virtual RenderDeviceType GetDeviceType() = 0;

        virtual Ref<Nova::RenderTarget> CreateRenderTarget(const RenderTargetCreateInfo& createInfo) = 0;
        virtual Ref<Texture> CreateTexture(const TextureCreateInfo& createInfo) = 0;
        virtual Ref<Texture> CreateTextureUnitialized() = 0;
        virtual Ref<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) = 0;
        virtual Ref<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) = 0;
        virtual Ref<Shader> CreateShader(const ShaderCreateInfo& createInfo) = 0;
        virtual Ref<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) = 0;
        virtual Ref<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) = 0;
        virtual Ref<Material> CreateMaterial(const MaterialCreateInfo& createInfo) = 0;
        virtual Ref<Fence> CreateFence(const FenceCreateInfo& createInfo) = 0;
        virtual Nova::CommandBuffer* GetCurrentCommandBuffer() { return nullptr; }

        Ref<Fence> CreateFence();
        Ref<Buffer> CreateBuffer(BufferUsage usage, size_t size);
        Ref<Texture> CreateTexture(TextureUsageFlags usage, uint32_t width, uint32_t height, Format format);
        Ref<Material> CreateMaterial(Ref<Shader> material);
        Ref<ComputePipeline> CreateComputePipeline(Ref<Shader> shader);
        Ref<Sampler> CreateSampler();
        Ref<Sampler> GetOrCreateSampler(const SamplerCreateInfo& createInfo);

        virtual uint32_t GetImageCount() const = 0;

        StringView GetDeviceVendor() const;
        bool HasVSync() const;
    protected:
        String m_DeviceVendor;
        bool m_VSync = false;
    private:
        Map<SamplerCreateInfo, Ref<Sampler>> m_Samplers;
    };

    Ref<RenderDevice> CreateRenderDevice(RenderDeviceType type, const DeviceCreateInfo& createInfo);
}
