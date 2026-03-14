#pragma once
#include "Containers/String.h"
#include "Containers/StringView.h"
#include "Containers/Map.h"
#include "Runtime/Ref.h"
#include "Runtime/Object.h"
#include "Runtime/Format.h"
#include "Runtime/LogCategory.h"
#include "SwpchainBuffering.h"
#include "RenderDeviceType.h"
#include "BufferUsage.h"
#include "TextureUsage.h"
#include "Sampler.h"

#include <cstdint>

NOVA_DECLARE_LOG_CATEGORY_STATIC(RenderDevice, "RENDER DEVICE")

namespace Nova
{
    class Window;
    class Surface;
    struct SurfaceCreateInfo;
    class Texture;
    struct TextureCreateInfo;
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
    class TextureView;
    struct TextureViewCreateInfo;
    class Queue;
    struct TextureBarrier;

    struct RenderDeviceCreateInfo
    {
        String appName;
        Window* window = nullptr;
        SwapchainBuffering buffering = SwapchainBuffering::DoubleBuffering;
        bool vSync = false;
    };

    class RenderDevice : public Object
    {
    public:
        RenderDevice() : Object("RenderDevice")
        {
            if (!s_Instance) s_Instance = this;
        }

        ~RenderDevice() override { s_Instance = nullptr; }

        virtual bool Initialize(const RenderDeviceCreateInfo& createInfo) = 0;
        virtual void Destroy();

        virtual bool BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Present() = 0;
        virtual void WaitIdle() const = 0;
        virtual void SetName(StringView name) = 0;

        virtual Nova::Surface* GetSurface() { return nullptr; }
        virtual Nova::Swapchain* GetSwapchain() { return nullptr; }
        virtual Nova::Queue* GetGraphicsQueue() { return nullptr; }
        virtual Nova::Queue* GetComputeQueue() { return nullptr; }
        virtual Nova::Queue* GetTransferQueue() { return nullptr; }
        virtual RenderDeviceType GetDeviceType() = 0;

        virtual Ref<Nova::RenderTarget> CreateRenderTarget(const RenderTargetCreateInfo& createInfo);
        virtual Ref<Nova::Texture> CreateTexture(const TextureCreateInfo& createInfo) = 0;
        virtual Ref<Nova::Texture> CreateTextureUnitialized() = 0;
        virtual Ref<Nova::TextureView> CreateTextureView(const TextureViewCreateInfo& createInfo) = 0;
        virtual Ref<Nova::Sampler> CreateSampler(const SamplerCreateInfo& createInfo) = 0;
        virtual Ref<Nova::Buffer> CreateBuffer(const BufferCreateInfo& createInfo) = 0;
        virtual Ref<Nova::Shader> CreateShader(const ShaderCreateInfo& createInfo) = 0;
        virtual Ref<Nova::GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) = 0;
        virtual Ref<Nova::ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) = 0;
        virtual Ref<Nova::Material> CreateMaterial(const MaterialCreateInfo& createInfo) = 0;
        virtual Ref<Nova::Fence> CreateFence(const FenceCreateInfo& createInfo) = 0;
        virtual Nova::CommandBuffer* GetCurrentCommandBuffer() { return nullptr; }

        Ref<Nova::Fence> CreateFence();
        Ref<Nova::Buffer> CreateBuffer(BufferUsage usage, size_t size);
        Ref<Nova::Texture> CreateTexture(TextureUsageFlags usage, uint32_t width, uint32_t height, Format format);
        Ref<Nova::Material> CreateMaterial(Ref<Shader> material);
        Ref<Nova::ComputePipeline> CreateComputePipeline(Ref<Shader> shader);
        Ref<Nova::Sampler> CreateSampler();
        Ref<Nova::Sampler> GetOrCreateSampler(const SamplerCreateInfo& createInfo);

        virtual Ref<Nova::CommandBuffer> CreateCommandBuffer() = 0;
        virtual Ref<Nova::CommandBuffer> CreateTransferCommandBuffer() = 0;

        virtual uint32_t GetImageCount() const = 0;
        virtual uint32_t GetCurrentFrameIndex() const = 0;

        StringView GetDeviceVendor() const;
        bool HasVSync() const;

        static Ref<RenderDevice> GetInstance();

        static void ImmediateTextureBarrier(const TextureBarrier& barrier);
    protected:
        String m_DeviceVendor;
        bool m_VSync = false;
    private:
        Map<SamplerCreateInfo, Ref<Nova::Sampler>> m_Samplers;
        static inline RenderDevice* s_Instance = nullptr;
    };

    Ref<RenderDevice> CreateRenderDevice(RenderDeviceType type, const RenderDeviceCreateInfo& createInfo);
}
