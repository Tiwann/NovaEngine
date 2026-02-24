#pragma once
#include "Containers/Lazy.h"
#include "Rendering/RenderTarget.h"
#include "Texture.h"

namespace Nova::D3D12
{
    class RenderTarget final : public Nova::RenderTarget
    {
    public:
        RenderTarget() = default;
        ~RenderTarget() override = default;

        bool Initialize(const RenderTargetCreateInfo& createInfo) override;
        void Destroy() override;
        bool Resize(uint32_t newX, uint32_t newY) override;
        const Texture& GetColorTexture() override;
        const Texture& GetDepthTexture() override;
    private:
        ID3D12Image* m_ColorImages[3] { nullptr, nullptr, nullptr };
        ID3D12ImageView* m_ColorImageViews[3] { nullptr, nullptr, nullptr };
        ID3D12Allocation* m_ColorAllocations[3] { nullptr, nullptr, nullptr };
        ID3D12Image* m_DepthImages[3] { nullptr, nullptr, nullptr };
        ID3D12ImageView* m_DepthImageViews[3] { nullptr, nullptr, nullptr };
        ID3D12Allocation* m_DepthAllocations[3] { nullptr, nullptr, nullptr };
        Lazy<Texture> m_ColorTexture;
        Lazy<Texture> m_DepthTexture;
    };
}
