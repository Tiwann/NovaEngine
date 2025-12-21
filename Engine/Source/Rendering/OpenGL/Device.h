#pragma once
#include "Rendering/Device.h"

namespace Nova::OpenGL
{
    class Device final : public Nova::Device
    {
    public:
        bool Initialize(const DeviceCreateInfo& createInfo) override;
        void Destroy() override;
        bool BeginFrame() override;
        void EndFrame() override;
        void Present() override;
        void WaitIdle() const override;
        void SetName(StringView name) override;
        DeviceType GetDeviceType() override;
        Ref<Nova::RenderTarget> CreateRenderTarget(const RenderTargetCreateInfo& createInfo) override;
        Ref<Nova::Surface> CreateSurface(const SurfaceCreateInfo& createInfo) override;
        Ref<Nova::Texture> CreateTexture(const TextureCreateInfo& createInfo) override;
        Ref<Nova::Texture> CreateTextureUnitialized() override;
        Ref<Nova::Sampler> CreateSampler(const SamplerCreateInfo& createInfo) override;
        Ref<Nova::Buffer> CreateBuffer(const BufferCreateInfo& createInfo) override;
        Ref<Nova::Shader> CreateShader(const ShaderCreateInfo& createInfo) override;
        Ref<Nova::GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) override;
        Ref<Nova::ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        Ref<Nova::Material> CreateMaterial(const MaterialCreateInfo& createInfo) override;
        Ref<Nova::Fence> CreateFence(const FenceCreateInfo& createInfo) override;
        uint32_t GetImageCount() const override;
    };
}
