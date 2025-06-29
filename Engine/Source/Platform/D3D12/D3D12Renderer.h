﻿#pragma once
#include "Rendering/Renderer.h"
#include "Runtime/LogCategory.h"
#include "Runtime/Image.h"
#include "D3D12RendererTypeConvertor.h"

#include <directx/d3d12.h>
#include <dxgi1_6.h>

NOVA_DECLARE_LOG_CATEGORY_STATIC(D3D12, "D3D12");
#define NOVA_DIRECTX_ERROR(message, ...) NOVA_LOG(D3D12, Verbosity::Error, message, __VA_ARGS__)
#define NOVA_DIRECTX_WARNING(message, ...) NOVA_LOG(D3D12, Verbosity::Warning, message, __VA_ARGS__)
#define DX_FAILED(Result) FAILED(Result)

namespace Nova
{
    template<typename T>
    using D3D12ComPtr = Microsoft::WRL::ComPtr<T>;

    struct D3D12FrameData
    {
        D3D12ComPtr<ID3D12Resource>             RenderTarget = nullptr;
        u64                                                RenderTargetViewHandle = 0;
        D3D12ComPtr<ID3D12GraphicsCommandList>  GraphicsCommandBuffer = nullptr;
        D3D12ComPtr<ID3D12CommandAllocator>     CommandAllocator = nullptr;
        D3D12ComPtr<ID3D12Fence>                Fence = nullptr;
        u64                                                FenceValue = 0;
    };
    
    class D3D12Renderer : public Renderer
    {
    public:
        D3D12Renderer(Application* Owner) : Renderer(Owner, GraphicsApi::D3D12) {}
        ~D3D12Renderer() override = default;

        bool Initialize(const RendererCreateInfo& CreateInfo) override;
        void Destroy() override;
        void ClearDepth(float Depth) override;
        void ClearColor(const Color& Color) override;
        void Present() override;
        bool BeginFrame() override;
        void BeginRendering() override;
        void EndRendering() override;
        void EndFrame() override;
        void SetViewport(const Viewport& Viewport) override;
        void SetScissor(const Scissor& Scissor) override;
        void DrawIndexed(size_t IndexCount, size_t Offset) override;

        void SetCullMode(CullMode Mode) override;
        void SetDepthCompareOperation(CompareOperation DepthFunction) override;
        void SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation, BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation) override;
        void SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation) override;
        void SetBlending(bool Enabled) override;
        void BindPipeline(Pipeline* Pipeline) override;

        void UpdateUniformBuffer(UniformBuffer* Buffer, u64 Offset, u64 Size, const void* Data) override;

        ID3D12GraphicsCommandList*  CreateOneTimeCommandBuffer() const;
        ID3D12Resource*             CreateBuffer(D3D12_HEAP_TYPE Type, D3D12_RESOURCE_STATES ResourceStates, size_t Size) const;
        ID3D12Resource*             CreateTexture2D(const WideString& Name, u32 Width, u32 Height, Format Format) const;
        ID3D12Device9*              GetDevice() const;
        IDXGIFactory7*              GetFactory() const;
        u32                         GetImageCount() const;
        ID3D12GraphicsCommandList*  GetCurrentGraphicsCommandBuffer() const;
        ID3D12Resource*             GetCurrentRenderTarget() const;
        ID3D12DescriptorHeap*       GetRenderTargetViewDescriptorHeap() const;
        ID3D12DescriptorHeap*       GetImGuiDescriptorHeap() const;
        ID3D12CommandAllocator*     GetCurrentCommandAllocator() const;
        ID3D12CommandQueue*         GetCommandQueue() const;
        void                        WaitIdle() const override;
        void BindVertexBuffer(VertexBuffer* Buffer, u64 Offset) override;
        void BindIndexBuffer(IndexBuffer* Buffer, u64 Offset) override;

        D3D12RendererTypeConvertor  Convertor;
    private:
        u32                         m_ImageCount = 0;
        u32                         m_CurrentFrameIndex = 0;
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        D3D12ComPtr<ID3D12Debug6>               m_Debug = nullptr;
        D3D12ComPtr<ID3D12InfoQueue1>           m_InfoQueue = nullptr;
#endif
        D3D12ComPtr<ID3D12Device9>              m_Device = nullptr;
        D3D12ComPtr<IDXGIFactory7>              m_Factory = nullptr;
        D3D12ComPtr<IDXGIAdapter>               m_Adapter = nullptr;
        DXGI_SWAP_EFFECT            m_PresentMode = DXGI_SWAP_EFFECT_DISCARD;
        D3D12ComPtr<IDXGISwapChain4>            m_Swapchain = nullptr;
        D3D12ComPtr<ID3D12CommandQueue>         m_CommandQueue = nullptr;
        D3D12ComPtr<ID3D12DescriptorHeap>       m_RenderTargetViewDescriptorHeap = nullptr;
        D3D12ComPtr<ID3D12DescriptorHeap>       m_ImGuiFontDescriptorHeap = nullptr;
        D3D12FrameData              m_FrameData[3] = {};
        HANDLE                      m_FenceEvent = nullptr;
    };
}
