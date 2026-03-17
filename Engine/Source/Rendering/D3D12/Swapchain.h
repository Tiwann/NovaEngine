#pragma once
#include "Rendering/Swapchain.h"
#include "Texture.h"
#include "TextureView.h"

struct IDXGISwapChain4;
struct ID3D12Resource;
struct ID3D12DescriptorHeap;
typedef ID3D12Resource ID3D12Image;
typedef void ID3D12ImageView;

namespace Nova::D3D12
{
    class Swapchain final : public Nova::Swapchain
    {
    public:
        bool Initialize(const SwapchainCreateInfo& createInfo) override;
        void Destroy() override;
        bool Recreate() override;
        void SetName(StringView name) override;

        IDXGISwapChain4* GetHandle();
        const IDXGISwapChain4* GetHandle() const;
        uint32_t AcquireNextFrame() const;
        ID3D12Resource* GetImage(size_t index) const;
        ID3D12ImageView* GetImageView(size_t index) const;

        const Nova::Texture* GetTexture() override;
        const Nova::TextureView* GetTextureView() override;

    private:
        IDXGISwapChain4* m_Handle = nullptr;
        ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
        ID3D12Image* m_Images[3] = {nullptr, nullptr, nullptr};
        ID3D12ImageView* m_ImageViews[3] = {nullptr, nullptr, nullptr};
        Texture m_Textures[3];
        TextureView m_TextureViews[3];
    };
}
