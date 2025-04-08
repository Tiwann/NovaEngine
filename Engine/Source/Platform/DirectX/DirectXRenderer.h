#pragma once
#include "Graphics/Renderer.h"
#include "Runtime/LogCategory.h"
#include <directx/d3dx12.h>
#include <dxgi1_6.h>

#include "Runtime/Image.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(DirectX, "D3D12");
#define NOVA_DIRECTX_ERROR(message, ...) NOVA_LOG(DirectX, Verbosity::Error, message, __VA_ARGS__)
#define NOVA_DIRECTX_WARNING(message, ...) NOVA_LOG(DirectX, Verbosity::Warning, message, __VA_ARGS__)
#define DX_FAILED(Result) FAILED(Result)
    
namespace Nova
{

    struct D3D12FrameData
    {
        ID3D12Resource*             RenderTarget = nullptr;
        u64                         RenderTargetViewHandle = 0;
        ID3D12GraphicsCommandList*  GraphicsCommandBuffer = nullptr;
        ID3D12CommandAllocator*     CommandAllocator = nullptr;
        ID3D12Fence*                Fence = nullptr;
        u64                         FenceValue = 0;
    };
    
    class DirectXRenderer : public Renderer
    {
    public:
        DirectXRenderer(Application* Owner) : Renderer(Owner) {}
        ~DirectXRenderer() override = default;

        bool Initialize() override;
        void Destroy() override;
        void ClearDepth(float Depth) override;
        void ClearColor(const Color& Color) override;
        void Present() override;
        bool BeginFrame() override;
        void EndFrame() override;
        void SetViewportRect(Vector2 Position, Vector2 Size) override;
        void Draw(DrawMode Mode, VertexArray* VAO, u32 NumVert, Shader* Shader) override;
        void DrawIndexed(DrawMode Mode, VertexArray* VertexArray, VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer,Shader* Shader) override;
        void DrawLine(const Vector3& PosA, const Vector3& PosB, f32 Thickness, const Color& Color) override;
        void DrawWireQuad(const Matrix4& Transform, const Vector3& Position, const Vector2& HalfExtents, f32 Thickness,const Color& Color) override;
        void DrawCircle(const Matrix4& Transform, const Vector3& Position, f32 Radius, const Color& Color) override;
        void Blit() override;
        void SetCullMode(CullMode Mode) override;
        void SetDepthFunction(DepthFunction DepthFunction) override;
        void SetBlendFunction(BlendMode ColorSource, BlendMode ColorDest, BlendOperation ColorOperation,BlendMode AlphaSource, BlendMode AlphaDest, BlendOperation AlphaOperation) override;
        void SetBlendFunction(BlendMode Source, BlendMode Destination, BlendOperation Operation) override;
        void SetBlending(bool Enabled) override;

        ID3D12GraphicsCommandList*  CreateOneTimeCommandBuffer() const;
        ID3D12Resource*             CreateBuffer(const WideString& Name, D3D12_HEAP_TYPE Type, D3D12_RESOURCE_STATES ResourceStates, size_t Size) const;
        ID3D12Resource*             CreateTexture2D(const WideString& Name, u32 Width, u32 Height, ImageFormat Format) const;
    public:
        ID3D12Device9*              GetDevice() const { return m_Device; }
        u32                         GetImageCount() const { return m_ImageCount; }
        ID3D12GraphicsCommandList*  GetCurrentGraphicsCommandBuffer() const { return m_FrameData[m_CurrentFrameIndex].GraphicsCommandBuffer; }
        ID3D12Resource*             GetCurrentRenderTarget() const { return m_FrameData[m_CurrentFrameIndex].RenderTarget; }
        ID3D12DescriptorHeap*       GetRenderTargetViewDescriptorHeap() const { return m_RenderTargetViewDescriptorHeap; }
        ID3D12DescriptorHeap*       GetImGuiDescriptorHeap() const { return m_ImGuiFontDescriptorHeap; }
        ID3D12CommandAllocator*     GetCurrentCommandAllocator() const { return m_FrameData[m_CurrentFrameIndex].CommandAllocator; }
        ID3D12CommandQueue*         GetCommandQueue() const { return m_CommandQueue; }
        void                        WaitDeviceIdle();
    private:
        u32                         m_ImageCount = 0;
        u32                         m_CurrentFrameIndex = 0;
#if defined(NOVA_DEBUG)
        ID3D12Debug6*               m_Debug = nullptr;
        ID3D12InfoQueue1*            m_InfoQueue = nullptr;
#endif
        ID3D12Device9*              m_Device = nullptr;
        IDXGIFactory7*              m_Factory = nullptr;
        IDXGIAdapter*               m_Adapter = nullptr;
        DXGI_SWAP_EFFECT            m_PresentMode;
        IDXGISwapChain4*            m_Swapchain = nullptr;
        ID3D12CommandQueue*         m_CommandQueue = nullptr;
        ID3D12DescriptorHeap*       m_RenderTargetViewDescriptorHeap = nullptr;
        ID3D12DescriptorHeap*       m_ImGuiFontDescriptorHeap = nullptr;
        D3D12FrameData              m_FrameData[NOVA_MAX_FRAMES_IN_FLIGHT] = {};
        HANDLE                      m_FenceEvent = nullptr;
    };
}
