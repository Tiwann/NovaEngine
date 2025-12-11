#pragma once
#include "Rendering/Fence.h"

struct ID3D12Fence1;

namespace Nova::D3D12
{
    class Fence final : public Nova::Fence
    {
    public:
        bool Initialize(const FenceCreateInfo& createInfo) override;
        void Destroy() override;
        void Wait(uint64_t timeout) override;
        void Reset() override;

        uint64_t GetValue() const { return m_Value; }
        ID3D12Fence1* GetHandle() { return m_Handle; }
        const ID3D12Fence1* GetHandle() const { return m_Handle; }
    private:
        ID3D12Fence1* m_Handle = nullptr;
        void* m_Event = nullptr;
        uint64_t m_Value = 0;
    };
}
