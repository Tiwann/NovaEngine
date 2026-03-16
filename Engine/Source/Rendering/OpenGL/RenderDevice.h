#pragma once
#include "Rendering/RenderDevice.h"
#include "Swapchain.h"
#include "Surface.h"
#include "Queue.h"
#include "CommandBuffer.h"

namespace Nova::OpenGL
{
    class RenderDevice final : public Nova::RenderDevice
    {
    public:
        bool Initialize(const RenderDeviceCreateInfo& createInfo) override;
        void Destroy() override;
        bool BeginFrame() override;
        void EndFrame() override;
        void Present() override;
        void WaitIdle() const override;
        void SetName(StringView name) override;
        RenderDeviceType GetDeviceType() override;

        Ref<Nova::RenderTarget> CreateRenderTarget(const RenderTargetCreateInfo& createInfo) override;
        Ref<Nova::Texture> CreateTexture(const TextureCreateInfo& createInfo) override;
        Ref<Nova::Texture> CreateTextureUnitialized() override;
        Ref<Nova::TextureView> CreateTextureView(const TextureViewCreateInfo& createInfo) override;
        Ref<Nova::Sampler> CreateSampler(const SamplerCreateInfo& createInfo) override;
        Ref<Nova::Buffer> CreateBuffer(const BufferCreateInfo& createInfo) override;
        Ref<Nova::Shader> CreateShader(const ShaderCreateInfo& createInfo) override;
        Ref<Nova::GraphicsPipeline> CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& createInfo) override;
        Ref<Nova::ComputePipeline> CreateComputePipeline(const ComputePipelineCreateInfo& createInfo) override;
        Ref<Nova::Material> CreateMaterial(const MaterialCreateInfo& createInfo) override;
        Ref<Nova::Fence> CreateFence(const FenceCreateInfo& createInfo) override;
        uint32_t GetImageCount() const override;

        Nova::Swapchain* GetSwapchain() override;
        Nova::CommandBuffer* GetCurrentCommandBuffer() override;
        Queue* GetGraphicsQueue() override { return &m_GraphicsQueue; }
        Queue* GetComputeQueue() override { return &m_ComputeQueue; }
        Queue* GetTransferQueue() override { return &m_TransferQueue; }

        Ref<Nova::CommandBuffer> CreateCommandBuffer() override;
        Ref<Nova::CommandBuffer> CreateTransferCommandBuffer() override;
        uint32_t GetCurrentFrameIndex() const override;

    private:
        Surface m_Surface;
        Swapchain m_Swapchain;
        CommandBuffer m_CommandBuffers[3];
        Queue m_GraphicsQueue{QueueType::Graphics};
        Queue m_TransferQueue{QueueType::Transfer};
        Queue m_ComputeQueue{QueueType::Compute};
        uint32_t m_CurrentFrameIndex = 0;
    };
}
