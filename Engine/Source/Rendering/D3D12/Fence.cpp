#include "Fence.h"
#include "RenderDevice.h"
#include <directx/d3d12.h>

namespace Nova::D3D12
{
    bool Fence::Initialize(const FenceCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        ID3D12Device13* deviceHandle = device->GetHandle();

        const HANDLE event = CreateEventA(nullptr, FALSE, FALSE, nullptr);
        if (!event) return false;
        if (m_Event) CloseHandle(m_Event);
        m_Event = event;

        ID3D12Fence1* fence = nullptr;
        if (DX_FAILED(deviceHandle->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
            return false;

        if (m_Handle) m_Handle->Release();
        if (DX_FAILED(fence->QueryInterface(IID_PPV_ARGS(&m_Handle))))
            return false;

        return true;
    }

    void Fence::Destroy()
    {
        if (m_Handle) m_Handle->Release();
        if (m_Event) CloseHandle(m_Event);
    }

    void Fence::Wait(uint64_t timeout)
    {
        if (m_Handle->GetCompletedValue() < m_Value)
        {
            m_Handle->SetEventOnCompletion(m_Value, m_Event);
            WaitForSingleObject(m_Handle, timeout);
        }
    }

    void Fence::Reset()
    {
    }
}
