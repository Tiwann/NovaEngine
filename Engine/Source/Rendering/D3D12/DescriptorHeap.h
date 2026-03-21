#pragma once
#include "Containers/Array.h"
#include <cstdint>


struct ID3D12DescriptorHeap;

namespace Nova::D3D12
{
    class RenderDevice;

    enum class DescriptorType
    {
        ShaderResourceView,
        UnorderedAccessView,
        ConstantBufferView,
        RenderTargetView,
        DepthStencilView,
    };

    enum class DescriptorHeapType
    {
        None,
        SRV_CBV_UAV,
        RTV,
        DSV,
        SamplerState
    };

    struct DescriptorHandle
    {
        uint32_t index = 0;
        uintptr_t cpu = 0;
        uintptr_t gpu = 0;

        DescriptorHandle() = default;
        DescriptorHandle(decltype(nullptr)) {}
    };

    struct DescriptorHeapCreateInfo
    {
        RenderDevice* device = nullptr;
        DescriptorHeapType heapType = DescriptorHeapType::None;
        uint32_t numDescriptors = 0;
        bool shaderVisible = false;
    };

    class DescriptorHeap
    {
    public:
        DescriptorHeap() = default;
        ~DescriptorHeap() = default;

        bool Initialize(const DescriptorHeapCreateInfo& createInfo);
        void Destroy();

        ID3D12DescriptorHeap* GetHandle() const;

        DescriptorHeapType GetHeapType() const;
        uint32_t GetNumDescriptors() const;
        bool IsShaderVisible() const;
        uintptr_t GetCpuStart() const;
        uintptr_t GetGpuStart() const;

        DescriptorHandle AllocateDescriptor();
        void FreeDescriptor(DescriptorHandle descriptor);
    private:
        RenderDevice* m_Device = nullptr;
        ID3D12DescriptorHeap* m_Handle = nullptr;
        DescriptorHeapType m_HeapType = DescriptorHeapType::None;
        uint32_t m_NumDescriptors = 0;
        bool m_ShaderVisible = false;
        uintptr_t m_CpuStart = 0;
        uintptr_t m_GpuStart = 0;
        Array<uint32_t> m_FreeList;
        uint32_t m_NextFree = 0;
    };
}