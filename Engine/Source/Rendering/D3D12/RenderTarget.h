#pragma once
#include "Rendering/RenderTarget.h"

namespace Nova::D3D12
{
    class RenderTarget final : public Nova::RenderTarget
    {
    public:
        bool Initialize(const RenderTargetCreateInfo& createInfo) override;
        void Destroy() override;
        bool Resize(uint32_t newX, uint32_t newY) override;
    };
}
