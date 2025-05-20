#include "D3D12Fence.h"

namespace Nova
{
    D3D12Fence::D3D12Fence(Renderer* Owner): Fence(Owner)
    {
    }

    bool D3D12Fence::Initialize(const FenceCreateInfo& CreateInfo)
    {
        return false;
    }

    void D3D12Fence::Destroy()
    {
    }

    void D3D12Fence::Reset()
    {
    }
}
