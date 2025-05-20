#pragma once
#include "D3D12Renderer.h"
#include "Rendering/Fence.h"

namespace Nova
{
    class D3D12Fence : public Fence
    {
    public:
        explicit D3D12Fence(Renderer* Owner);

        bool Initialize(const FenceCreateInfo& CreateInfo) override;
        void Destroy() override;
        void Reset() override;
    private:
        ID3D12Fence* m_Handle = nullptr;
    };
}
