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
        bool Resize(size_t newSize, bool keepData) override;
        bool CPUCopy(const void* src, size_t offset, size_t size) override;
        bool CPUCopy(size_t offset, size_t size, void* outBuffer) override;
        bool GPUCopy(Nova::Buffer& other, size_t srcOffset, size_t destOffset, size_t size) override;
        void Memset(size_t value, size_t size) override;
    private:
        RenderDevice* m_Device = nullptr;
        ID3D12Allocation* m_Allocation = nullptr;
        ID3D12Resource* m_Handle = nullptr;
    };
}
