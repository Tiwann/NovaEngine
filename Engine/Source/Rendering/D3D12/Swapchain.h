#pragma once
#include "Rendering/Swapchain.h"

struct IDXGISwapChain4;
struct ID3D12Resource;
struct ID3D12DescriptorHeap;
typedef ID3D12Resource ID3D12Image;
typedef uint64_t ID3D12ImageView;

namespace Nova::D3D12
{
    class Swapchain final : public Nova::Swapchain
    {
    public:
        bool Initialize(const SwapchainCreateInfo& createInfo) override;
        void Destroy() override;
        bool Recreate() override;
        void SetName(StringView name) override;

        IDXGISwapChain4* GetHandle() { return m_Handle; }
        const IDXGISwapChain4* GetHandle() const { return m_Handle; }
        uint32_t AcquireNextFrame();
        ID3D12Resource* GetImage(size_t index) { return m_Images[index]; }
        ID3D12ImageView GetImageView(size_t index) { return m_ImageViewsHandles[index]; }
    private:
        IDXGISwapChain4* m_Handle = nullptr;
        ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
        ID3D12Image* m_Images[3] = {nullptr, nullptr, nullptr};
        ID3D12ImageView m_ImageViewsHandles[3] = {0, 0, 0};
    };
}
