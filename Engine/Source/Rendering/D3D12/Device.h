#pragma once
#include "Rendering/Device.h"
#include "Swapchain.h"
#include "Surface.h"
#include "Queue.h"
#include "Fence.h"
#include "CommandBuffer.h"
#include "CommandPool.h"

#define DX_FAILED(result) ((result) < 0)

struct ID3D12Device13;
struct ID3D12Debug6;
struct ID3D12InfoQueue1;
struct IDXGIFactory7;
struct IDXGIAdapter4;
struct IDXGISwapChain4;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;

namespace D3D12MA { class Allocator; }
typedef D3D12MA::Allocator ID3D12Allocator;

namespace Nova::D3D12
{
    struct Frame
    {
        Fence fence;
        CommandBuffer commandBuffer;
    };

    class Device final : public Nova::Device
    {
    public:
        bool Initialize(const Nova::DeviceCreateInfo& createInfo) override;
        void Destroy() override;
        bool BeginFrame() override;
        void EndFrame() override;
        void Present() override;
        void WaitIdle() const override;
        void SetName(Nova::StringView name) override;

        Nova::DeviceType GetDeviceType() override;

        Ref<Nova::RenderTarget> CreateRenderTarget(const RenderTargetCreateInfo& createInfo) override;
        Ref<Nova::Surface> CreateSurface(const Nova::SurfaceCreateInfo& createInfo) override;
        Ref<Nova::Texture> CreateTexture(const Nova::TextureCreateInfo& createInfo) override;
        Ref<Nova::Texture> CreateTextureUnitialized() override;
        Ref<Nova::Sampler> CreateSampler(const Nova::SamplerCreateInfo& createInfo) override;
        Ref<Nova::Buffer> CreateBuffer(const Nova::BufferCreateInfo& createInfo) override;
        Ref<Nova::Shader> CreateShader(const Nova::ShaderCreateInfo& createInfo) override;
        Ref<Nova::GraphicsPipeline> CreateGraphicsPipeline(const Nova::GraphicsPipelineCreateInfo& createInfo) override;
        Ref<Nova::ComputePipeline> CreateComputePipeline(const Nova::ComputePipelineCreateInfo& createInfo) override;
        Ref<Nova::Material> CreateMaterial(const Nova::MaterialCreateInfo& createInfo) override;
        Ref<Nova::Fence> CreateFence(const Nova::FenceCreateInfo& createInfo) override;
        uint32_t GetImageCount() const override;

        ID3D12Device13* GetHandle() { return m_Handle; }
        const ID3D12Device13* GetHandle() const { return m_Handle; }

        IDXGIFactory7* GetFactory() { return m_Factory; }
        const IDXGIFactory7* GetFactory() const { return m_Factory; }

        IDXGIAdapter4* GetAdapter() { return m_Adapter; }
        const IDXGIAdapter4* GetAdapter() const { return m_Adapter; }

        Queue* GetGraphicsQueue() { return &m_GraphicsQueue; }
        const Queue* GetGraphicsQueue() const { return &m_GraphicsQueue; }
        Queue* GetComputeQueue() { return &m_ComputeQueue; }
        const Queue* GetComputeQueue() const { return &m_ComputeQueue; }
        Queue* GetTransferQueue() { return &m_TransferQueue; }
        const Queue* GetTransferQueue() const { return &m_TransferQueue; }

        CommandBuffer& GetCurrentCommandBuffer();
        CommandPool& GetCommandPool();
        ID3D12Allocator* GetAllocator();
        Nova::Swapchain* GetSwapchain() override;
        uint32_t GetCurrentFrameIndex() const;

    private:
        uint32_t m_ImageCount = 0;
        uint32_t m_CurrentFrameIndex = 0;
        uint32_t m_LastFrameIndex = 0;

        ID3D12Device13* m_Handle = nullptr;
        IDXGIFactory7* m_Factory = nullptr;
        IDXGIAdapter4* m_Adapter = nullptr;
        ID3D12Allocator* m_Allocator = nullptr;

        Surface m_Surface;
        Swapchain m_Swapchain;
        Queue m_GraphicsQueue;
        Queue m_ComputeQueue;
        Queue m_TransferQueue;
        CommandPool m_CommandPool;
        Frame m_Frames[NOVA_MAX_IMAGE_COUNT];

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        ID3D12Debug6* m_Debug = nullptr;
        ID3D12InfoQueue1* m_InfoQueue = nullptr;
#endif
    };
}
