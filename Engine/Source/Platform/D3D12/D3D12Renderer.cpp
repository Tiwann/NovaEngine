#include "D3D12Renderer.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/VertexBuffer.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <directx/d3dx12.h>


namespace Nova
{
#if defined(NOVA_DEBUG)
    static void DebugCallback(D3D12_MESSAGE_CATEGORY Category, D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID ID, LPCSTR Description, void* pContext)
    {
        switch (Severity)
        {
            case D3D12_MESSAGE_SEVERITY_ERROR:
            case D3D12_MESSAGE_SEVERITY_CORRUPTION:
                NOVA_DIRECTX_ERROR("Direct3D Error: {}", Description);
                return;
            case D3D12_MESSAGE_SEVERITY_WARNING:
                NOVA_DIRECTX_WARNING("Direct3D Warning: {}", Description);
        default:
            break;
        }
    }
#endif
    
    bool D3D12Renderer::Initialize()
    {
        m_ImageCount = (u32)m_Application->GetGraphicsSettings().BufferType;
        
        // Creating factory
        if (DX_FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_Factory))))
        {
            NOVA_DIRECTX_ERROR("Failed to create Factory!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }

        // Selecting Adapter
        UINT AdapterIndex = 0;
        IDXGIAdapter4* Adapter = nullptr;
        while (m_Factory->EnumAdapters1(AdapterIndex, (IDXGIAdapter1**)&Adapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC1 Description;
            (void)Adapter->GetDesc1(&Description);
            if (Description.DedicatedVideoMemory > 0)
            {
                m_Adapter = Adapter;
                break;
            }           
            ++AdapterIndex;
        }


#if defined(NOVA_DEBUG)
        if (DX_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debug))))
        {
            NOVA_DIRECTX_ERROR("Failed to get debug interface!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }

        m_Debug->EnableDebugLayer();
#endif

        
        // Creating device
        if (DX_FAILED(D3D12CreateDevice(m_Adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device))))
        {
            NOVA_DIRECTX_ERROR("Failed to create Device! Maybe your GPU doesn't support D3D12 Feature Level 12.2 (D3D_FEATURE_LEVEL_12_2)");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }


#if defined(NOVA_DEBUG)
        if (DX_FAILED(m_Device->QueryInterface(IID_PPV_ARGS(&m_InfoQueue))))
        {
            NOVA_DIRECTX_ERROR("Failed to get D3D12 Info Queue interface!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }
        
        m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
        m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);
        
        D3D12_INFO_QUEUE_FILTER Filter = {};
        m_InfoQueue->PushStorageFilter(&Filter);
        
        DWORD callbackCookie;
        if (DX_FAILED(m_InfoQueue->RegisterMessageCallback(DebugCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &callbackCookie)))
        {
            NOVA_DIRECTX_ERROR("Failed to register debug callback!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }
#endif

        // Creating Command Queue
        D3D12_COMMAND_QUEUE_DESC CommandQueueDesc = {};
        CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        if (DX_FAILED(m_Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(&m_CommandQueue))))
        {
            NOVA_DIRECTX_ERROR("Failed to create Command Queue!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }

        // Create Swapchain
        Window* Window = g_Application->GetWindow();
        const u32 WindowWidth = Window->GetWidth<u32>();
        const u32 WindowHeight = Window->GetHeight<u32>();

        const GraphicsSettings& GraphicsSettings = m_Application->GetGraphicsSettings();
        if (GraphicsSettings.VSync)
        {
            m_PresentMode = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            BOOL MailboxSupported = false;
            m_Factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &MailboxSupported, sizeof(MailboxSupported));
            if (MailboxSupported)
                m_PresentMode = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        } else
        {
            m_PresentMode = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        }
        
        DXGI_SWAP_CHAIN_DESC1 SwapchainDesc = {};
        SwapchainDesc.BufferCount = m_ImageCount;
        SwapchainDesc.Width = WindowWidth;
        SwapchainDesc.Height = WindowHeight;
        SwapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        SwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        SwapchainDesc.SwapEffect = m_PresentMode;
        SwapchainDesc.SampleDesc.Count = 1;

        // Not currently supporting fullscreen
        /*DXGI_SWAP_CHAIN_FULLSCREEN_DESC SwapchainFullscreenDesc = {};
        SwapchainFullscreenDesc.Windowed = false;
        SwapchainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        SwapchainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;*/

        const HWND Win32Window = glfwGetWin32Window(Window->GetNativeWindow());
        if (DX_FAILED(m_Factory->CreateSwapChainForHwnd(m_CommandQueue, Win32Window, &SwapchainDesc, nullptr, nullptr, (IDXGISwapChain1**)&m_Swapchain)))
        {
            NOVA_DIRECTX_ERROR("Failed to create Swapchain!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }


        D3D12_DESCRIPTOR_HEAP_DESC RenderTargetViewHeapDesc = {};
        RenderTargetViewHeapDesc.NumDescriptors = m_ImageCount;
        RenderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        if (DX_FAILED(m_Device->CreateDescriptorHeap(&RenderTargetViewHeapDesc, IID_PPV_ARGS(&m_RenderTargetViewDescriptorHeap))))
        {
            NOVA_DIRECTX_ERROR("Failed to create Descriptor Heap!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }
        
        for (UINT i = 0; i < m_ImageCount; ++i)
        {
            D3D12FrameData& CurrentFrame = m_FrameData[i];
            ID3D12Resource** RenderTarget = &CurrentFrame.RenderTarget;
            u64& RenderTargetHandle = CurrentFrame.RenderTargetViewHandle;

            if (DX_FAILED(m_Swapchain->GetBuffer(i, IID_PPV_ARGS(RenderTarget))))
            {
                NOVA_DIRECTX_ERROR("Failed to get Render Target!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            const UINT Size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            RenderTargetHandle = m_RenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;
            RenderTargetHandle += (SIZE_T)(i * Size);
            m_Device->CreateRenderTargetView(*RenderTarget, nullptr, { RenderTargetHandle });

            if (DX_FAILED(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_FrameData[i].CommandAllocator))))
            {
                NOVA_DIRECTX_ERROR("Failed to create Command Allocator!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }
            
            if (DX_FAILED(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameData[i].CommandAllocator, nullptr, IID_PPV_ARGS(&m_FrameData[i].GraphicsCommandBuffer))))
            {
                NOVA_DIRECTX_ERROR("Failed to create Command List!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }

            m_FrameData[i].GraphicsCommandBuffer->Close();

            if (DX_FAILED(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_FrameData[i].Fence))))
            {
                NOVA_DIRECTX_ERROR("Failed to create Fence!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }
            m_FenceEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr);
        }

        if (m_Application->GetConfiguration().WithEditor)
        {
            D3D12_DESCRIPTOR_HEAP_DESC ImGuiFontDescriptorHeapDesc = {};
            Memory::Memzero(ImGuiFontDescriptorHeapDesc);
            ImGuiFontDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            ImGuiFontDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            ImGuiFontDescriptorHeapDesc.NumDescriptors = 1;
            if (DX_FAILED(m_Device->CreateDescriptorHeap(&ImGuiFontDescriptorHeapDesc, IID_PPV_ARGS(&m_ImGuiFontDescriptorHeap))))
            {
                NOVA_DIRECTX_ERROR("Failed to create Descriptor Heap for ImGui font!");
                m_Application->RequireExit(ExitCode::Error);
                return false;
            }
        }
        
        return true;
    }

    void D3D12Renderer::Destroy()
    {
    }

    void D3D12Renderer::ClearDepth(float Depth)
    {
        
    }

    void D3D12Renderer::ClearColor(const Color& Color)
    {
        const LONG WindowWidth = g_Application->GetWindow()->GetWidth<LONG>();
        const LONG WindowHeight = g_Application->GetWindow()->GetHeight<LONG>();
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        const u64 RenderTargetViewHandle = m_FrameData[m_CurrentFrameIndex].RenderTargetViewHandle;

        const D3D12_RECT ClearRect = { 0, 0, WindowWidth, WindowHeight };
        Cmd->ClearRenderTargetView({RenderTargetViewHandle}, (const f32*)&Color, 0, &ClearRect);
    }

    bool D3D12Renderer::BeginFrame()
    {
        if (ShouldRecreateSwapchain)
        {
            WaitDeviceIdle();
            
            for (UINT i = 0; i < m_ImageCount; i++)
            {
                D3D12FrameData& FrameData = m_FrameData[i];
                FrameData.GraphicsCommandBuffer->Release();
                FrameData.CommandAllocator->Release();
                FrameData.RenderTarget->Release();
                FrameData.RenderTargetViewHandle = 0;
                FrameData.FenceValue = 0;
            }

            const UINT WindowWidth = g_Application->GetWindow()->GetWidth<UINT>();
            const UINT WindowHeight = g_Application->GetWindow()->GetHeight<UINT>();
            
            DXGI_SWAP_CHAIN_DESC OldSwapchainDesc = {};
            m_Swapchain->GetDesc(&OldSwapchainDesc);
            m_Swapchain->ResizeBuffers(m_ImageCount, WindowWidth, WindowHeight, OldSwapchainDesc.BufferDesc.Format, OldSwapchainDesc.Flags);

            for (UINT i = 0; i < m_ImageCount; i++)
            {
                if (DX_FAILED(m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&m_FrameData[i].RenderTarget))))
                {
                    NOVA_DIRECTX_ERROR("Failed to get Render Target!");
                    m_Application->RequireExit(ExitCode::Error);
                    return false;
                }

                const UINT Size = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                m_FrameData[i].RenderTargetViewHandle = m_RenderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr;
                m_FrameData[i].RenderTargetViewHandle += (SIZE_T)(i * Size);
                m_Device->CreateRenderTargetView(m_FrameData[i].RenderTarget, nullptr, {m_FrameData[i].RenderTargetViewHandle});

                if (DX_FAILED(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_FrameData[i].CommandAllocator))))
                {
                    NOVA_DIRECTX_ERROR("Failed to create Command Allocator!");
                    m_Application->RequireExit(ExitCode::Error);
                    return false;
                }
            
                if (DX_FAILED(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameData[i].CommandAllocator, nullptr, IID_PPV_ARGS(&m_FrameData[i].GraphicsCommandBuffer))))
                {
                    NOVA_DIRECTX_ERROR("Failed to create Command List!");
                    m_Application->RequireExit(ExitCode::Error);
                    return false;
                }

                m_FrameData[i].GraphicsCommandBuffer->Close();

                if (DX_FAILED(m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_FrameData[i].Fence))))
                {
                    NOVA_DIRECTX_ERROR("Failed to create Fence!");
                    m_Application->RequireExit(ExitCode::Error);
                    return false;
                }
                m_FenceEvent = CreateEventA(nullptr, FALSE, FALSE, nullptr);
            }
            
            m_CurrentFrameIndex = m_Swapchain->GetCurrentBackBufferIndex();
            ShouldRecreateSwapchain = false;
            return false;
        }
        
        ID3D12CommandAllocator* Allocator = m_FrameData[m_CurrentFrameIndex].CommandAllocator;
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        ID3D12Resource* RenderTarget = GetCurrentRenderTarget();
        const u64& RenderTargetViewHandle = m_FrameData[m_CurrentFrameIndex].RenderTargetViewHandle;
        
        if (DX_FAILED(Allocator->Reset()))
        {
            NOVA_DIRECTX_ERROR("Failed to reset Command Allocator!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }
        
        if (DX_FAILED(Cmd->Reset(Allocator, nullptr)))
        {
            NOVA_DIRECTX_ERROR("Failed to reset Command Buffer!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }
        
        const D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        RenderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        Cmd->ResourceBarrier(1, &Barrier);
        const D3D12_CPU_DESCRIPTOR_HANDLE RenderTargets[] { { RenderTargetViewHandle } };
        Cmd->OMSetRenderTargets(1, RenderTargets, false, nullptr);
        return true;
    }

    void D3D12Renderer::EndFrame()
    {
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        ID3D12Resource* RenderTarget = GetCurrentRenderTarget();
        const D3D12_RESOURCE_BARRIER Barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        RenderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        Cmd->ResourceBarrier(1, &Barrier);
        Cmd->Close();
        m_CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList**)&Cmd);
        
        const UINT64 FenceValue = m_FrameData[m_CurrentFrameIndex].FenceValue;
        if (DX_FAILED(m_CommandQueue->Signal(m_FrameData[m_CurrentFrameIndex].Fence, FenceValue)))
        {
            NOVA_DIRECTX_ERROR("Failed to signal Fence!");
            m_Application->RequireExit(ExitCode::Error);
        }
    }

    void D3D12Renderer::Present()
    {
        if (const GraphicsSettings& Settings = g_Application->GetGraphicsSettings(); DX_FAILED(m_Swapchain->Present(Settings.VSync, 0)))
        {
            NOVA_DIRECTX_ERROR("Failed to present!");
            m_Application->RequireExit(ExitCode::Error);
            return;
        }
        
        m_CurrentFrameIndex = m_Swapchain->GetCurrentBackBufferIndex();
        ID3D12Fence* Fence = m_FrameData[m_CurrentFrameIndex].Fence;
        UINT64& FenceValue = m_FrameData[m_CurrentFrameIndex].FenceValue;

        if (Fence->GetCompletedValue() < FenceValue)
        {
            Fence->SetEventOnCompletion(FenceValue, m_FenceEvent);
            WaitForSingleObject(m_FenceEvent, U32_MAX);
        }
        
        FenceValue++;
    }

    void D3D12Renderer::WaitDeviceIdle()
    {
        ID3D12Fence* Fence = m_FrameData[m_CurrentFrameIndex].Fence;
        UINT64& FenceValue = m_FrameData[m_CurrentFrameIndex].FenceValue;
        m_CommandQueue->Signal(Fence, FenceValue);

        if (Fence->GetCompletedValue() < FenceValue)
        {
            Fence->SetEventOnCompletion(FenceValue, m_FenceEvent);
            WaitForSingleObject(m_FenceEvent, U32_MAX);
        }
    }

    void D3D12Renderer::SetViewportRect(Vector2 Position, Vector2 Size)
    {
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        const D3D12_VIEWPORT Viewport = CD3DX12_VIEWPORT(Position.x, Position.y, Position.x + Size.x, Position.y + Size.y);
        Cmd->RSSetViewports(1, &Viewport);
    }

    void D3D12Renderer::Draw(DrawMode Mode, VertexArray* VAO, u32 NumVert, Shader* Shader)
    {
    }

    void D3D12Renderer::DrawIndexed(DrawMode Mode, VertexArray* VertexArray, VertexBuffer* VertexBuffer, IndexBuffer* IndexBuffer, Shader* Shader)
    {
        VertexBuffer->Bind();
        IndexBuffer->Bind();

        D3D12_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        switch (Mode)
        {
        case DrawMode::Points:
            Topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        case DrawMode::Lines:
            Topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case DrawMode::LineStrip:
            Topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
            break;

        case DrawMode::Triangles:
            Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        }
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        Cmd->DrawIndexedInstanced((UINT)IndexBuffer->Count(), 1, 0, 0, 0);
        Cmd->IASetPrimitiveTopology(Topology);
    }

    void D3D12Renderer::DrawLine(const Vector3& PosA, const Vector3& PosB, f32 Thickness, const Color& Color)
    {
    }

    void D3D12Renderer::DrawWireQuad(const Matrix4& Transform, const Vector3& Position, const Vector2& HalfExtents,
        f32 Thickness, const Color& Color)
    {
    }

    void D3D12Renderer::DrawCircle(const Matrix4& Transform, const Vector3& Position, f32 Radius, const Color& Color)
    {
    }

    void D3D12Renderer::Blit()
    {
    }

    void D3D12Renderer::SetCullMode(CullMode Mode)
    {
        
    }

    void D3D12Renderer::SetDepthFunction(DepthFunction DepthFunction)
    {
    }

    void D3D12Renderer::SetBlendFunction(BlendMode ColorSource, BlendMode ColorDest, BlendOperation ColorOperation,
        BlendMode AlphaSource, BlendMode AlphaDest, BlendOperation AlphaOperation)
    {
    }

    void D3D12Renderer::SetBlendFunction(BlendMode Source, BlendMode Destination, BlendOperation Operation)
    {
    }

    void D3D12Renderer::SetBlending(bool Enabled)
    {
    }

    ID3D12GraphicsCommandList* D3D12Renderer::CreateOneTimeCommandBuffer() const
    {
        ID3D12CommandAllocator* CommandAllocator = GetCurrentCommandAllocator();
        ID3D12GraphicsCommandList* Cmd = nullptr;
        if (DX_FAILED(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator, nullptr, IID_PPV_ARGS(&Cmd))))
        {
            return nullptr;
        }
        Cmd->SetName(L"One-Time Command Buffer");
        return Cmd;
    }


    ID3D12Resource* D3D12Renderer::CreateBuffer(const WideString& Name, D3D12_HEAP_TYPE Type, D3D12_RESOURCE_STATES ResourceStates, size_t Size) const
    {
        const CD3DX12_HEAP_PROPERTIES HeapProps(Type);
        const CD3DX12_RESOURCE_DESC BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(Size);
        ID3D12Resource* OutResource = nullptr;
        if (DX_FAILED(m_Device->CreateCommittedResource(
            &HeapProps,
            D3D12_HEAP_FLAG_NONE,
            &BufferDesc,
            ResourceStates,
            nullptr,
            IID_PPV_ARGS(&OutResource)
        )))
        {
            return nullptr;
        }

        OutResource->SetName(*Name);
        return OutResource;
    }

    ID3D12Resource* D3D12Renderer::CreateTexture2D(const WideString& Name, u32 Width, u32 Height, Formats Format) const
    {
        DXGI_FORMAT DXFormat = DXGI_FORMAT_UNKNOWN;
        switch (Format)
        {
        case Formats::NONE:
            DXFormat =  DXGI_FORMAT_UNKNOWN;
            break;
        case Formats::R8_UNORM:
            DXFormat = DXGI_FORMAT_R8_UNORM;
            break;
        case Formats::R8_SNORM:
            DXFormat = DXGI_FORMAT_R8_SNORM;
            break;
        case Formats::R16_USHORT:
            DXFormat = DXGI_FORMAT_R16_UINT;
            break;
        case Formats::R16_SHORT:
            DXFormat = DXGI_FORMAT_R16_SINT;
            break;
        case Formats::R32_FLOAT:
            DXFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        case Formats::R32_UINT:
            DXFormat = DXGI_FORMAT_R32_UINT;
            break;
        case Formats::R32_SINT:
            DXFormat = DXGI_FORMAT_R32_SINT;
            break;
        case Formats::R8G8_UNORM:
            DXFormat = DXGI_FORMAT_R8G8_UNORM;
            break;
        case Formats::R8G8_SNORM:
            DXFormat = DXGI_FORMAT_R8G8_SNORM;
            break;
        case Formats::R16G16_USHORT:
            DXFormat = DXGI_FORMAT_R16G16_UINT;
            break;
        case Formats::R16G16_SHORT:
            DXFormat = DXGI_FORMAT_R16G16_SINT;
            break;
        case Formats::R32G32_UINT:
            DXFormat = DXGI_FORMAT_R32G32_UINT;
            break;
        case Formats::R32G32_SINT:
            DXFormat = DXGI_FORMAT_R32G32_SINT;
            break;
        case Formats::R32G32_FLOAT:
            DXFormat = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case Formats::R8G8B8_UNORM:
        case Formats::R8G8B8_SNORM:
        case Formats::R16G16B16_USHORT:
        case Formats::R16G16B16_SHORT:
        case Formats::R32G32B32_UINT:
        case Formats::R32G32B32_SINT:
        case Formats::R32G32B32_FLOAT:
            break;
        case Formats::R8G8B8A8_UNORM:
            DXFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case Formats::R8G8B8A8_SNORM:
            DXFormat = DXGI_FORMAT_R8G8B8A8_SNORM;
            break;
        case Formats::R16G16B16A16_USHORT:
            DXFormat = DXGI_FORMAT_R16G16B16A16_UINT;
            break;
        case Formats::R16G16B16A16_SHORT:
            DXFormat = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case Formats::R32G32B32A32_UINT:
            DXFormat = DXGI_FORMAT_R32G32B32A32_UINT;
            break;
        case Formats::R32G32B32A32_SINT:
            DXFormat = DXGI_FORMAT_R32G32B32A32_SINT;
            break;
        case Formats::R32G32B32A32_FLOAT:
            DXFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            break;
        }

        const CD3DX12_HEAP_PROPERTIES HeapProperties(D3D12_HEAP_TYPE_DEFAULT);
        const CD3DX12_RESOURCE_DESC ResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXFormat, Width, Height);

        ID3D12Resource* OutResource = nullptr;
        if (DX_FAILED(m_Device->CreateCommittedResource(&HeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &ResourceDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&OutResource))))
        {
            return nullptr;
        }
        OutResource->SetName(*Name);
        return OutResource;
    }
}

