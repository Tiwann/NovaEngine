#include "D3D12Renderer.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"
#include "Runtime/Log.h"
#include "D3D12Pipeline.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/VertexBuffer.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <directx/d3dx12.h>

#include "D3D12VertexBuffer.h"
#include "Runtime/DesktopWindow.h"


namespace Nova
{
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
    static void DebugCallback(D3D12_MESSAGE_CATEGORY Category, const D3D12_MESSAGE_SEVERITY Severity, D3D12_MESSAGE_ID ID, const LPCSTR Description, void* pContext)
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
    
    bool D3D12Renderer::Initialize(const RendererCreateInfo& CreateInfo)
    {
        m_ImageCount = (u32)m_Application->GetGraphicsSettings().Buffering;
        
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


#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        if (DX_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debug))))
        {
            NOVA_DIRECTX_ERROR("Failed to get debug interface!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }

        m_Debug->EnableDebugLayer();
#endif

        
        // Creating device
        if (DX_FAILED(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device))))
        {
            NOVA_DIRECTX_ERROR("Failed to create Device! Maybe your GPU doesn't support D3D12 Feature Level 12.2 (D3D_FEATURE_LEVEL_12_2)");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }


#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
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
        if (DX_FAILED(m_Device->CreateCommandQueue(&CommandQueueDesc, IID_PPV_ARGS(m_CommandQueue.GetAddressOf()))))
        {
            NOVA_DIRECTX_ERROR("Failed to create Command Queue!");
            m_Application->RequireExit(ExitCode::Error);
            return false;
        }

        // Create Swapchain
        Window* Window = g_Application->GetWindow();
        const u32 WindowWidth = Window->GetWidth();
        const u32 WindowHeight = Window->GetHeight();

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

        const HWND Win32Window = glfwGetWin32Window(Window->As<DesktopWindow>()->GetHandle());
        if (DX_FAILED(m_Factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), Win32Window, &SwapchainDesc, nullptr, nullptr, (IDXGISwapChain1**)m_Swapchain.GetAddressOf())))
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
            
            if (DX_FAILED(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameData[i].CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_FrameData[i].GraphicsCommandBuffer))))
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
        WaitIdle();

    }

    void D3D12Renderer::ClearDepth(float Depth)
    {
        
    }

    void D3D12Renderer::ClearColor(const Color& Color)
    {
        const LONG WindowWidth = g_Application->GetWindow()->GetWidth();
        const LONG WindowHeight = g_Application->GetWindow()->GetHeight();
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        const u64 RenderTargetViewHandle = m_FrameData[m_CurrentFrameIndex].RenderTargetViewHandle;

        const D3D12_RECT ClearRect = { 0, 0, WindowWidth, WindowHeight };
        Cmd->ClearRenderTargetView({RenderTargetViewHandle}, (const f32*)&Color, 0, &ClearRect);
    }

    bool D3D12Renderer::BeginFrame()
    {
        if (ShouldRecreateSwapchain)
        {
            WaitIdle();
            
            for (UINT i = 0; i < m_ImageCount; i++)
            {
                D3D12FrameData& FrameData = m_FrameData[i];
                FrameData.GraphicsCommandBuffer->Release();
                FrameData.CommandAllocator->Release();
                FrameData.RenderTarget->Release();
                FrameData.RenderTargetViewHandle = 0;
                FrameData.FenceValue = 0;
            }

            const UINT WindowWidth = g_Application->GetWindow()->GetWidth();
            const UINT WindowHeight = g_Application->GetWindow()->GetHeight();
            
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
                m_Device->CreateRenderTargetView(m_FrameData[i].RenderTarget.Get(), nullptr, {m_FrameData[i].RenderTargetViewHandle});

                if (DX_FAILED(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_FrameData[i].CommandAllocator))))
                {
                    NOVA_DIRECTX_ERROR("Failed to create Command Allocator!");
                    m_Application->RequireExit(ExitCode::Error);
                    return false;
                }
            
                if (DX_FAILED(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameData[i].CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_FrameData[i].GraphicsCommandBuffer))))
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
        
        ID3D12CommandAllocator* Allocator = m_FrameData[m_CurrentFrameIndex].CommandAllocator.Get();
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

    void D3D12Renderer::BeginRendering()
    {
    }

    void D3D12Renderer::EndRendering()
    {
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
        if (DX_FAILED(m_CommandQueue->Signal(m_FrameData[m_CurrentFrameIndex].Fence.Get(), FenceValue)))
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
        ID3D12Fence* Fence = m_FrameData[m_CurrentFrameIndex].Fence.Get();
        UINT64& FenceValue = m_FrameData[m_CurrentFrameIndex].FenceValue;

        if (Fence->GetCompletedValue() < FenceValue)
        {
            Fence->SetEventOnCompletion(FenceValue, m_FenceEvent);
            WaitForSingleObject(m_FenceEvent, U32_MAX);
        }
        
        FenceValue++;
    }

    void D3D12Renderer::WaitIdle() const
    {
        ID3D12Fence* Fence = m_FrameData[m_CurrentFrameIndex].Fence.Get();
        const UINT64& FenceValue = m_FrameData[m_CurrentFrameIndex].FenceValue;
        m_CommandQueue->Signal(Fence, FenceValue);

        if (Fence->GetCompletedValue() < FenceValue)
        {
            Fence->SetEventOnCompletion(FenceValue, m_FenceEvent);
            WaitForSingleObject(m_FenceEvent, U32_MAX);
        }
    }

    void D3D12Renderer::BindVertexBuffer(VertexBuffer* Buffer, u64 Offset)
    {

    }

    void D3D12Renderer::BindIndexBuffer(IndexBuffer* Buffer, u64 Offset)
    {
    }

    void D3D12Renderer::SetViewport(const Viewport& Viewport)
    {
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        const D3D12_VIEWPORT Result = CD3DX12_VIEWPORT(Viewport.X, Viewport.Y, Viewport.X + Viewport.Width, Viewport.Y + Viewport.Height, Viewport.MinDepth, Viewport.MaxDepth);
        Cmd->RSSetViewports(1, &Result);
    }

    void D3D12Renderer::SetScissor(const Scissor& Scissor)
    {
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        const D3D12_RECT Rect = CD3DX12_RECT(Scissor.X, Scissor.Y, Scissor.X + Scissor.Width, Scissor.Y + Scissor.Height);
        Cmd->RSSetScissorRects(1, &Rect);
    }

    void D3D12Renderer::Draw(size_t VertexCount, size_t FirstVertex)
    {
    }


    void D3D12Renderer::DrawIndexed(const size_t IndexCount, size_t Offset)
    {
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        Cmd->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
    }


    void D3D12Renderer::SetCullMode(CullMode Mode)
    {
        
    }

    void D3D12Renderer::SetDepthCompareOperation(CompareOperation DepthFunction)
    {
    }

    void D3D12Renderer::SetBlendFunction(BlendFactor ColorSource, BlendFactor ColorDest, BlendOperation ColorOperation,BlendFactor AlphaSource, BlendFactor AlphaDest, BlendOperation AlphaOperation)
    {
    }

    void D3D12Renderer::SetBlendFunction(BlendFactor Source, BlendFactor Destination, BlendOperation Operation)
    {
    }

    void D3D12Renderer::SetBlending(bool Enabled)
    {
    }

    void D3D12Renderer::BindPipeline(Pipeline* Pipeline)
    {
        ID3D12GraphicsCommandList* Cmd = GetCurrentGraphicsCommandBuffer();
        Cmd->SetPipelineState(Pipeline->As<D3D12Pipeline>()->GetHandle());
    }

    void D3D12Renderer::UpdateUniformBuffer(UniformBuffer* Buffer, u64 Offset, u64 Size, const void* Data)
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


    ID3D12Resource* D3D12Renderer::CreateBuffer(const D3D12_HEAP_TYPE Type, const D3D12_RESOURCE_STATES ResourceStates, const size_t Size) const
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

        return OutResource;
    }

    ID3D12Resource* D3D12Renderer::CreateTexture2D(const WideString& Name, const u32 Width, const u32 Height, const Format Format) const
    {
        DXGI_FORMAT DXFormat = DXGI_FORMAT_UNKNOWN;
        switch (Format)
        {
        case Format::None:
            DXFormat =  DXGI_FORMAT_UNKNOWN;
            break;
        case Format::R8_UNORM:
            DXFormat = DXGI_FORMAT_R8_UNORM;
            break;
        case Format::R8_SNORM:
            DXFormat = DXGI_FORMAT_R8_SNORM;
            break;
        case Format::R16_USHORT:
            DXFormat = DXGI_FORMAT_R16_UINT;
            break;
        case Format::R16_SHORT:
            DXFormat = DXGI_FORMAT_R16_SINT;
            break;
        case Format::R32_FLOAT:
            DXFormat = DXGI_FORMAT_R32_FLOAT;
            break;
        case Format::R32_UINT:
            DXFormat = DXGI_FORMAT_R32_UINT;
            break;
        case Format::R32_SINT:
            DXFormat = DXGI_FORMAT_R32_SINT;
            break;
        case Format::R8G8_UNORM:
            DXFormat = DXGI_FORMAT_R8G8_UNORM;
            break;
        case Format::R8G8_SNORM:
            DXFormat = DXGI_FORMAT_R8G8_SNORM;
            break;
        case Format::R16G16_USHORT:
            DXFormat = DXGI_FORMAT_R16G16_UINT;
            break;
        case Format::R16G16_SHORT:
            DXFormat = DXGI_FORMAT_R16G16_SINT;
            break;
        case Format::R32G32_UINT:
            DXFormat = DXGI_FORMAT_R32G32_UINT;
            break;
        case Format::R32G32_SINT:
            DXFormat = DXGI_FORMAT_R32G32_SINT;
            break;
        case Format::R32G32_FLOAT:
            DXFormat = DXGI_FORMAT_R32G32_FLOAT;
            break;
        case Format::R8G8B8_UNORM:
        case Format::R8G8B8_SNORM:
        case Format::R16G16B16_USHORT:
        case Format::R16G16B16_SHORT:
        case Format::R32G32B32_UINT:
        case Format::R32G32B32_SINT:
        case Format::R32G32B32_FLOAT:
            break;
        case Format::R8G8B8A8_UNORM:
            DXFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            break;
        case Format::R8G8B8A8_SNORM:
            DXFormat = DXGI_FORMAT_R8G8B8A8_SNORM;
            break;
        case Format::R16G16B16A16_USHORT:
            DXFormat = DXGI_FORMAT_R16G16B16A16_UINT;
            break;
        case Format::R16G16B16A16_SHORT:
            DXFormat = DXGI_FORMAT_R16G16B16A16_SINT;
            break;
        case Format::R32G32B32A32_UINT:
            DXFormat = DXGI_FORMAT_R32G32B32A32_UINT;
            break;
        case Format::R32G32B32A32_SINT:
            DXFormat = DXGI_FORMAT_R32G32B32A32_SINT;
            break;
        case Format::R32G32B32A32_FLOAT:
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

    ID3D12Device9* D3D12Renderer::GetDevice() const
    {
        return m_Device.Get();
    }

    IDXGIFactory7* D3D12Renderer::GetFactory() const
    {
        return m_Factory.Get();
    }

    u32 D3D12Renderer::GetImageCount() const
    {
        return m_ImageCount;
    }

    ID3D12GraphicsCommandList* D3D12Renderer::GetCurrentGraphicsCommandBuffer() const
    {
        return m_FrameData[m_CurrentFrameIndex].GraphicsCommandBuffer.Get();
    }

    ID3D12Resource* D3D12Renderer::GetCurrentRenderTarget() const
    {
        return m_FrameData[m_CurrentFrameIndex].RenderTarget.Get();
    }

    ID3D12DescriptorHeap* D3D12Renderer::GetRenderTargetViewDescriptorHeap() const
    {
        return m_RenderTargetViewDescriptorHeap.Get();
    }

    ID3D12DescriptorHeap* D3D12Renderer::GetImGuiDescriptorHeap() const
    {
        return m_ImGuiFontDescriptorHeap.Get();
    }

    ID3D12CommandAllocator* D3D12Renderer::GetCurrentCommandAllocator() const
    {
        return m_FrameData[m_CurrentFrameIndex].CommandAllocator.Get();
    }

    ID3D12CommandQueue* D3D12Renderer::GetCommandQueue() const
    {
        return m_CommandQueue.Get();
    }
}

