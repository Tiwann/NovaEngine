#pragma once
#include "Rendering/Swapchain.h"

struct IDXGISwapChain4;

namespace Nova
{
    class D3D12Swapchain : public Swapchain
    {
    public:
        explicit D3D12Swapchain(Renderer* Owner);
        bool Initialize(const SwapchainCreateInfo& CreateInfo) override;
        void Destroy() override;
        bool Recreate() override;
    private:
        IDXGISwapChain4* m_Handle = nullptr;
    };
}

