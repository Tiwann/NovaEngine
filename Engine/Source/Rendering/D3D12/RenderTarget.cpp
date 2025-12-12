#include "RenderTarget.h"

namespace Nova::D3D12
{
    bool RenderTarget::Initialize(const RenderTargetCreateInfo& createInfo)
    {
        return false;
    }

    void RenderTarget::Destroy()
    {
    }

    bool RenderTarget::Resize(uint32_t newX, uint32_t newY)
    {
        return false;
    }
}
