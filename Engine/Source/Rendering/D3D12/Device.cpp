#include "Device.h"
#include "Containers/StringFormat.h"
#include <directx/d3dx12.h>
#include <dxgi1_6.h>
#include <D3D12MemAlloc.h>

using namespace D3D12MA;

namespace Nova::D3D12
{
#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
    static void DebugCallback(D3D12_MESSAGE_CATEGORY category, const D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, const LPCSTR desc, void* pContext)
    {
        switch (severity)
        {
        case D3D12_MESSAGE_SEVERITY_ERROR:
        case D3D12_MESSAGE_SEVERITY_CORRUPTION:
            std::println(std::cerr, "Direct3D Error: {}", desc);
            return;
        case D3D12_MESSAGE_SEVERITY_WARNING:
            std::println(std::cout, "Direct3D Warning: {}", desc);
        default:
            break;
        }
    }
#endif

    bool Device::Initialize(const DeviceCreateInfo& createInfo)
    {
        if (DX_FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_Factory))))
            return false;

        uint32_t adapterIndex = 0;
        IDXGIAdapter4* adapter = nullptr;
        while (m_Factory->EnumAdapters1(adapterIndex, (IDXGIAdapter1**)&adapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC1 description;
            if (DX_FAILED(adapter->GetDesc1(&description)))
                return false;

            if (description.DedicatedVideoMemory > 0)
            {
                m_Adapter = adapter;
                break;
            }
            ++adapterIndex;
        }

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        if (DX_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_Debug))))
            return false;
        m_Debug->EnableDebugLayer();
#endif
        if (DX_FAILED(D3D12CreateDevice(m_Adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Handle))))
            return false;

#if defined(NOVA_DEBUG) || defined(NOVA_DEV)
        if (DX_FAILED(m_Handle->QueryInterface(IID_PPV_ARGS(&m_InfoQueue))))
            return false;

        (void)m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, false);
        (void)m_InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, false);

        /*D3D12_INFO_QUEUE_FILTER filter = {};
        (void)m_InfoQueue->PushStorageFilter(&filter);*/

        DWORD callbackCookie = 0;
        if (DX_FAILED(m_InfoQueue->RegisterMessageCallback(DebugCallback, D3D12_MESSAGE_CALLBACK_FLAG_NONE, nullptr, &callbackCookie)))
            return false;
#endif

        const auto GetPresentMode = [this](bool vSync)
        {
            DXGI_SWAP_EFFECT result;

            if (vSync)
            {
                result = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
                BOOL mailboxSupported = false;
                (void)m_Factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &mailboxSupported, sizeof(mailboxSupported));
                if (mailboxSupported)
                    result = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            } else
            {
                result = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            }

            switch (result)
            {
                case DXGI_SWAP_EFFECT_DISCARD: return PresentMode::Immediate;
                case DXGI_SWAP_EFFECT_FLIP_DISCARD: return PresentMode::Fifo;
                default: return PresentMode::Unknown;
            }
        };

        SurfaceCreateInfo surfaceCreateInfo;
        surfaceCreateInfo.device = this;
        surfaceCreateInfo.window = createInfo.window;
        m_Surface.Initialize(surfaceCreateInfo);


        m_GraphicsQueue.Initialize(this, QueueType::Graphics);
        m_ComputeQueue.Initialize(this, QueueType::Compute);
        m_TransferQueue.Initialize(this, QueueType::Transfer);

        SwapchainCreateInfo swapchainCreateInfo;
        swapchainCreateInfo.device = this;
        swapchainCreateInfo.surface = &m_Surface;
        swapchainCreateInfo.recycle = false;
        swapchainCreateInfo.buffering = createInfo.buffering;
        swapchainCreateInfo.format = Format::R8G8B8A8_UNORM;
        swapchainCreateInfo.width = m_Surface.GetWidth();
        swapchainCreateInfo.height = m_Surface.GetHeight();
        swapchainCreateInfo.presentMode = GetPresentMode(createInfo.vSync);

        if (!m_Swapchain.Initialize(swapchainCreateInfo))
        {
            std::println(std::cerr, "Failed to create swapchain!");
            return false;
        }

        CommandPoolCreateInfo cmdPoolCreateInfo;
        cmdPoolCreateInfo.device = this;
        cmdPoolCreateInfo.flags = CommandPoolCreateFlagBits::None;
        cmdPoolCreateInfo.queue = &m_GraphicsQueue;
        if (!m_CommandPool.Initialize(cmdPoolCreateInfo))
            return false;

        for (uint32_t frameIndex = 0; frameIndex < m_Swapchain.GetImageCount(); ++frameIndex)
        {
            Fence& fence = m_Frames[frameIndex].fence;
            FenceCreateInfo fenceCreateInfo = FenceCreateInfo().WithDevice(this);
            if (!fence.Initialize(fenceCreateInfo))
                return false;

            CommandBuffer commandBuffer = m_CommandPool.AllocateCommandBuffer(CommandBufferLevel::Primary);
            commandBuffer.SetName(StringFormat("Main Commander Buffer ({})", frameIndex));
            m_Frames[frameIndex].commandBuffer = commandBuffer;
        }


        ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.pAdapter = m_Adapter;
        allocatorDesc.pDevice = m_Handle;
        allocatorDesc.pAllocationCallbacks = nullptr;
        allocatorDesc.Flags = ALLOCATOR_FLAG_SINGLETHREADED;
        if (DX_FAILED(CreateAllocator(&allocatorDesc, &m_Allocator)))
            return false;
        
        return false;
    }

    void Device::Destroy()
    {
    }

    bool Device::BeginFrame()
    {
        m_CurrentFrameIndex = m_Swapchain.AcquireNextFrame();
        CommandBuffer& cmdBuffer = GetCurrentCommandBuffer();
        ID3D12GraphicsCommandList10* cmdBufferHandle = cmdBuffer.GetHandle();
        ID3D12Image* swapchainImage = m_Swapchain.GetImage(m_CurrentFrameIndex);
        ID3D12ImageView swapchainImageView = m_Swapchain.GetImageView(m_CurrentFrameIndex);

        if (!cmdBuffer.Begin({CommandBufferUsageFlagBits::None}))
            return false;

        const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapchainImage, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        cmdBufferHandle->ResourceBarrier(1, &barrier);

        // This needs to be render passes
        const D3D12_CPU_DESCRIPTOR_HANDLE renderTargets[] { {swapchainImageView} };
        cmdBufferHandle->OMSetRenderTargets(1, renderTargets, false, nullptr);
        return false;
    }

    void Device::EndFrame()
    {
        CommandBuffer& cmdBuffer = GetCurrentCommandBuffer();
        Fence& fence = m_Frames[m_CurrentFrameIndex].fence;
        ID3D12GraphicsCommandList10* cmdBufferHandle = cmdBuffer.GetHandle();
        ID3D12Image* swapchainImage = m_Swapchain.GetImage(m_CurrentFrameIndex);

        const D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(swapchainImage, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        cmdBufferHandle->ResourceBarrier(1, &barrier);
        cmdBuffer.End();

        m_GraphicsQueue.Submit(&cmdBuffer, nullptr, nullptr, &fence, 0);
    }

    void Device::Present()
    {
        (void)m_GraphicsQueue.Present(m_Swapchain, nullptr, m_CurrentFrameIndex);
    }

    void Device::WaitIdle() const
    {
    }

    void Device::SetName(Nova::StringView name)
    {
    }

    Nova::DeviceType Device::GetDeviceType()
    {
        return Nova::DeviceType::D3D12;
    }

    Nova::Ref<Nova::Surface> Device::CreateSurface(const Nova::SurfaceCreateInfo& createInfo)
    {
        return nullptr;
    }

    Nova::Ref<Nova::Texture> Device::CreateTexture(const Nova::TextureCreateInfo& createInfo)
    {
        return nullptr;
    }

    Nova::Ref<Nova::Texture> Device::CreateTextureUnitialized()
    {
        return nullptr;
    }

    Nova::Ref<Nova::Sampler> Device::CreateSampler(const Nova::SamplerCreateInfo& createInfo)
    {
        return nullptr;
    }

    Nova::Ref<Nova::Buffer> Device::CreateBuffer(const Nova::BufferCreateInfo& createInfo)
    {
        return nullptr;
    }

    Nova::Ref<Nova::Shader> Device::CreateShader(const Nova::ShaderCreateInfo& createInfo)
    {
        return nullptr;
    }

    Nova::Ref<Nova::GraphicsPipeline> Device::CreateGraphicsPipeline(const Nova::GraphicsPipelineCreateInfo& createInfo)
    {
        return nullptr;
    }

    Nova::Ref<Nova::ComputePipeline> Device::CreateComputePipeline(const Nova::ComputePipelineCreateInfo& createInfo)
    {
        return nullptr;
    }

    Nova::Ref<Nova::Material> Device::CreateMaterial(const Nova::MaterialCreateInfo& createInfo)
    {
        return nullptr;
    }

    Nova::Ref<Nova::Fence> Device::CreateFence(const Nova::FenceCreateInfo& createInfo)
    {
        return nullptr;
    }

    uint32_t Device::GetImageCount() const
    {
        return 0;
    }

    CommandBuffer& Device::GetCurrentCommandBuffer()
    {
        return m_Frames[m_CurrentFrameIndex].commandBuffer;
    }

    CommandPool& Device::GetCommandPool()
    {
        return m_CommandPool;
    }

    D3D12MA::Allocator* Device::GetAllocator()
    {
        return m_Allocator;
    }
}
