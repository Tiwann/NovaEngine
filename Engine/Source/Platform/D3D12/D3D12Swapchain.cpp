#include "D3D12Swapchain.h"
#include "D3D12Renderer.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"

#include <directx/d3dx12.h>
#include <dxgi1_6.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


namespace Nova
{
    D3D12Swapchain::D3D12Swapchain(Renderer* Owner) : Swapchain(Owner)
    {

    }

    bool D3D12Swapchain::Initialize(const SwapchainCreateInfo& CreateInfo)
    {
        const D3D12Renderer* Renderer = m_Owner->As<D3D12Renderer>();
        Window* Window = Renderer->GetOwner()->GetWindow();
        GLFWwindow* NativeWindow = Window->GetNativeWindow();
        const HWND Hwnd = glfwGetWin32Window(NativeWindow);
        const auto& Convertor = Renderer->Convertor;

        DXGI_SWAP_CHAIN_DESC1 Desc { };
        Desc.BufferCount = CreateInfo.ImageCount;
        Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        Desc.SampleDesc.Count = 1;
        Desc.SwapEffect = Convertor.ConvertPresentMode(CreateInfo.ImagePresentMode);
        Desc.Format = Convertor.ConvertFormat(CreateInfo.ImageFormat);
        Desc.Width = CreateInfo.ImageWidth;
        Desc.Height = CreateInfo.ImageHeight;
        Desc.Scaling = DXGI_SCALING_NONE;
        Desc.Stereo = false;
        Desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

        IDXGIFactory7* Factory = Renderer->GetFactory();
        ID3D12Device* Device = Renderer->GetDevice();
        if (DX_FAILED(Factory->CreateSwapChainForHwnd(Device, Hwnd, &Desc, nullptr, nullptr, (IDXGISwapChain1**)&m_Handle)))
            return false;
        return true;
    }

    void D3D12Swapchain::Destroy()
    {
        m_Handle->Release();
        m_Handle = nullptr;
    }

    bool D3D12Swapchain::Recreate()
    {
        return false;
    }
}
