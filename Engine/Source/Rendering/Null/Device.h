#pragma once
#include "Rendering/Device.h"

namespace Nova::Null
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
        Ref<Surface> CreateSurface(const SurfaceCreateInfo& createInfo) override;
        Ref<Texture> CreateTexture(const TextureCreateInfo& createInfo) override;
        Ref<Texture> CreateTextureUnitialized() override;
        Ref<Sampler> CreateSampler(const SamplerCreateInfo& createInfo) override;
        Ref<Buffer> CreateBuffer(const BufferCreateInfo& createInfo) override;
        Ref<Shader> CreateShader(const ShaderCreateInfo& createInfo) override;
        Ref<GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) override;
        Ref<ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        Ref<Material> CreateMaterial(const MaterialCreateInfo& createInfo) override;
        Ref<Fence> CreateFence(const FenceCreateInfo& createInfo) override;
        uint32_t GetImageCount() const override;
    };
}
