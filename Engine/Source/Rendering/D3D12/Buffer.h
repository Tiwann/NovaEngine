#pragma once
#include "Rendering/Buffer.h"

namespace D3D12MA { class Allocation; }
struct ID3D12Resource;
typedef D3D12MA::Allocation ID3D12Allocation;

namespace Nova::D3D12
{
    class RenderDevice;

    class Buffer final : public Nova::Buffer
    {
    public:
        bool Initialize(const BufferCreateInfo& createInfo) override;
        void Destroy() override;
        void* Map() override;
        void Unmap(const void* ptr) override;
        ID3D12Resource* GetHandle() const;
        ID3D12Allocation* GetAllocation() const;
    private:
        RenderDevice* m_Device = nullptr;
        ID3D12Allocation* m_Allocation = nullptr;
        ID3D12Resource* m_Handle = nullptr;
    };
}
