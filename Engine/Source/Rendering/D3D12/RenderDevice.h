#pragma once
#include "Rendering/RenderDevice.h"
#include "Swapchain.h"
#include "Surface.h"
#include "Queue.h"
#include "Fence.h"
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "DescriptorHeap.h"

#define DX_FAILED(result) ((result) < 0)

struct ID3D12Device13;
struct ID3D12Debug6;
struct ID3D12InfoQueue1;
struct IDXGIFactory7;
struct IDXGIAdapter4;
struct IDXGISwapChain4;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12CommandSignature;

namespace D3D12MA { class Allocator; }
typedef D3D12MA::Allocator ID3D12Allocator;

namespace Nova::D3D12
{
    struct Frame
    {
        Fence fence;
        CommandBuffer commandBuffer;
    };

    class RenderDevice final : public Nova::RenderDevice
    {
    public:
        bool Initialize(const Nova::RenderDeviceCreateInfo& createInfo) override;
        void Destroy() override;
        bool BeginFrame() override;
        void EndFrame() override;
        void Present() override;
        void WaitIdle() const override;
        void SetName(Nova::StringView name) override;

        Nova::RenderDeviceType GetDeviceType() override;

        Ref<Nova::Texture> CreateTexture(const Nova::TextureCreateInfo& createInfo) override;
        Ref<Nova::Texture> CreateTextureUnitialized() override;
        Ref<Nova::TextureView> CreateTextureView(const TextureViewCreateInfo& createInfo) override;
        Ref<Nova::Sampler> CreateSampler(const Nova::SamplerCreateInfo& createInfo) override;
        Ref<Nova::Buffer> CreateBuffer(const Nova::BufferCreateInfo& createInfo) override;
        Ref<Nova::Shader> CreateShader(const Nova::ShaderCreateInfo& createInfo) override;
        Ref<Nova::GraphicsPipeline> CreateGraphicsPipeline(const Nova::GraphicsPipelineCreateInfo& createInfo) override;
        Ref<Nova::ComputePipeline> CreateComputePipeline(const Nova::ComputePipelineCreateInfo& createInfo) override;
        Ref<Nova::Material> CreateMaterial(const Nova::MaterialCreateInfo& createInfo) override;
        Ref<Nova::Fence> CreateFence(const Nova::FenceCreateInfo& createInfo) override;
        Ref<Nova::CommandBuffer> CreateCommandBuffer() override;
        Ref<Nova::CommandBuffer> CreateTransferCommandBuffer() override;
        Ref<Nova::CommandBuffer> CreateComputeCommandBuffer() override;

        uint32_t GetImageCount() const override;
        uint32_t GetCurrentFrameIndex() const override;

        ID3D12Device13* GetHandle() const;
        IDXGIFactory7* GetFactory() const;
        IDXGIAdapter4* GetAdapter() const;
        ID3D12CommandSignature* GetDrawIndirectSignature() const;
        ID3D12CommandSignature* GetDrawIndexedIndirectSignature() const;
        ID3D12CommandSignature* GetDispatchIndirectSignature() const;
        ID3D12Allocator* GetAllocator();

        Queue* GetGraphicsQueue() override { return &m_GraphicsQueue; }
        Queue* GetComputeQueue() override { return &m_ComputeQueue; }
        Queue* GetTransferQueue() override { return &m_TransferQueue; }

        Nova::CommandBuffer* GetCurrentCommandBuffer() override;
        Nova::Swapchain* GetSwapchain() override;


        DescriptorHeap* GetDescriptorHeap();
        DescriptorHeap* GetSamplerHeap();
    private:
        uint32_t m_ImageCount = 0;
        uint32_t m_CurrentFrameIndex = 0;
        uint32_t m_LastFrameIndex = 0;

        ID3D12Device13* m_Handle = nullptr;
        IDXGIFactory7* m_Factory = nullptr;
        IDXGIAdapter4* m_Adapter = nullptr;
        ID3D12Allocator* m_Allocator = nullptr;
        ID3D12CommandSignature* m_DrawIndirectSignature = nullptr;
        ID3D12CommandSignature* m_DrawIndexedIndirectSignature = nullptr;
        ID3D12CommandSignature* m_DispatchIndirectSignature = nullptr;

        Surface m_Surface;
        Swapchain m_Swapchain;
        Queue m_GraphicsQueue;
        Queue m_ComputeQueue;
        Queue m_TransferQueue;
        CommandPool m_CommandPool;
        CommandPool m_TransferPool;
        CommandPool m_ComputePool;
        Frame m_Frames[3];
        DescriptorHeap m_SamplerHeap;
        DescriptorHeap m_DescriptorHeap;
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        ID3D12Debug6* m_Debug = nullptr;
        ID3D12InfoQueue1* m_InfoQueue = nullptr;
#endif
    };
}
