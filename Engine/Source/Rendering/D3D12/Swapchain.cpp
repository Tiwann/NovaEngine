#include "Runtime/DesktopWindow.h"
#include "Swapchain.h"
#include "Conversions.h"
#include "RenderDevice.h"
#include "Surface.h"
#include "Texture.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <dxgi1_6.h>
#include <directx/d3d12.h>

namespace Nova::D3D12
{
    bool Swapchain::Initialize(const SwapchainCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.surface) return false;

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        Surface* surface = static_cast<Surface*>(createInfo.surface);
        DesktopWindow* window = static_cast<DesktopWindow*>(surface->GetWindow());
        IDXGIFactory7* factory = device->GetFactory();
        Queue* graphicsQueue = device->GetGraphicsQueue();

        DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
        swapchainDesc.Flags = 0;
        swapchainDesc.BufferCount = (uint32_t)createInfo.buffering;
        swapchainDesc.Width = createInfo.width;
        swapchainDesc.Height = createInfo.height;
        swapchainDesc.Format = Convert<DXGI_FORMAT>(createInfo.format);
        swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainDesc.SwapEffect = Convert<DXGI_SWAP_EFFECT>(createInfo.presentMode);
        swapchainDesc.SampleDesc.Count = 1;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapchainFullscreenDesc = {};
        swapchainFullscreenDesc.RefreshRate = DXGI_RATIONAL(0, 0); // Using native display refresh rate
        swapchainFullscreenDesc.Windowed = false;
        swapchainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapchainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

        const HWND hwnd = glfwGetWin32Window(window->GetHandle());
        IDXGISwapChain1* swapchain = nullptr;
        if (DX_FAILED(factory->CreateSwapChainForHwnd(graphicsQueue->GetHandle(), hwnd, &swapchainDesc, nullptr, nullptr, &swapchain)))
            return false;

        if (m_Handle) m_Handle->Release();

        if (DX_FAILED(swapchain->QueryInterface(IID_PPV_ARGS(&m_Handle))))
            return false;


        ID3D12Device13* deviceHandle = device->GetHandle();
        const uint32_t descriptorSize = deviceHandle->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
        renderTargetViewHeapDesc.NumDescriptors = (uint32_t)createInfo.buffering;
        renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

        if (DX_FAILED(deviceHandle->CreateDescriptorHeap(&renderTargetViewHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap))))
            return false;

        for (size_t imageIndex = 0; imageIndex < (size_t)createInfo.buffering; imageIndex++)
        {
            ID3D12Resource* image = nullptr;
            if (DX_FAILED(m_Handle->GetBuffer(imageIndex, IID_PPV_ARGS(&image))))
                return false;

            if (m_Images[imageIndex]) m_Images[imageIndex]->Release();
            if (DX_FAILED(image->QueryInterface(IID_PPV_ARGS(&m_Images[imageIndex]))))
                return false;

            m_ImageViews[imageIndex] = reinterpret_cast<void*>(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + imageIndex * descriptorSize);
            deviceHandle->CreateRenderTargetView(m_Images[imageIndex], nullptr, {reinterpret_cast<SIZE_T>(m_ImageViews[imageIndex])});

            m_Textures[imageIndex].SetDirty();
        }

        m_Device = createInfo.device;
        m_Surface = createInfo.surface;
        m_Buffering = createInfo.buffering;
        m_ImageWidth = createInfo.width;
        m_ImageHeight = createInfo.height;
        m_ImageFormat = createInfo.format;
        m_ImagePresentMode = createInfo.presentMode;
        return true;
    }

    void Swapchain::Destroy()
    {
        if (m_Handle) m_Handle->Release();
        for (size_t imageIndex = 0; imageIndex < m_Device->GetImageCount(); imageIndex++)
        {
            m_Images[imageIndex]->Release();
        }
    }

    bool Swapchain::Recreate()
    {
        const Nova::Surface* surface = GetSurface();
        const uint32_t width = surface->GetWidth();
        const uint32_t height = surface->GetHeight();
        const uint32_t bufferCount = GetImageCount();
        const DXGI_FORMAT format = Convert<DXGI_FORMAT>(GetFormat());
        if (DX_FAILED(m_Handle->ResizeBuffers(bufferCount, width, height, format, 0)))
            return false;

        return true;
    }

    void Swapchain::SetName(StringView name)
    {
    }

    Ref<Nova::Texture> Swapchain::GetTexture(const uint32_t index)
    {
        const auto createTexture = [this, &index]() -> Ref<Texture>
        {
            Ref<Texture> texture = new Texture();
            texture->m_Device = (RenderDevice*)m_Device;
            texture->m_Image = m_Images[index];
            texture->m_ImageView = m_ImageViews[index];
            texture->m_Width = m_ImageWidth;
            texture->m_Height = m_ImageHeight;
            texture->m_Allocation = nullptr;
            texture->m_SampleCount = 1;
            texture->m_Mips = 1;
            texture->m_Format = m_ImageFormat;
            return texture;
        };

        return m_Textures[index].Get(createTexture);
    }

    Ref<Nova::Texture> Swapchain::GetCurrentTexture()
    {
        const RenderDevice* device = (RenderDevice*)m_Device;
        const size_t imageIndex = device->GetCurrentFrameIndex();
        return GetTexture(imageIndex);
    }

    IDXGISwapChain4* Swapchain::GetHandle()
    {
        return m_Handle;
    }

    const IDXGISwapChain4* Swapchain::GetHandle() const
    {
        return m_Handle;
    }

    ID3D12Resource* Swapchain::GetImage(size_t index) const
    {
        return m_Images[index];
    }

    ID3D12ImageView* Swapchain::GetImageView(size_t index) const
    {
        return m_ImageViews[index];
    }

    uint32_t Swapchain::AcquireNextFrame()
    {
        return m_Handle->GetCurrentBackBufferIndex();
    }
}
