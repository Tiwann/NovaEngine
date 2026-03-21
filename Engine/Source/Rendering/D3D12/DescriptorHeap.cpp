#include "DescriptorHeap.h"
#include "RenderDevice.h"
#include <directx/d3dx12.h>


namespace Nova::D3D12
{
    static D3D12_DESCRIPTOR_HEAP_TYPE D3D12HeapType(const DescriptorHeapType heapType)
    {
        switch (heapType)
        {
        case DescriptorHeapType::SRV_CBV_UAV: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        case DescriptorHeapType::RTV: return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        case DescriptorHeapType::DSV: return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        case DescriptorHeapType::SamplerState: return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        default: return D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
        }
    }

    bool DescriptorHeap::Initialize(const DescriptorHeapCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (createInfo.numDescriptors == 0) return false;
        if (createInfo.heapType == DescriptorHeapType::None) return false;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.NumDescriptors = createInfo.numDescriptors;
        desc.Type = D3D12HeapType(createInfo.heapType);
        desc.Flags = createInfo.shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAGS(0);

        RenderDevice* device = createInfo.device;
        ID3D12Device13* deviceHandle = device->GetHandle();
        ID3D12DescriptorHeap* handle = nullptr;
        if (DX_FAILED(deviceHandle->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&handle))))
            return false;
        if (m_Handle) m_Handle->Release();
        m_Handle = handle;

        m_Device = device;
        m_HeapType = createInfo.heapType;
        m_NumDescriptors = createInfo.numDescriptors;
        m_ShaderVisible = createInfo.shaderVisible;
        m_CpuStart = m_Handle->GetCPUDescriptorHandleForHeapStart().ptr;
        m_GpuStart = m_Handle->GetGPUDescriptorHandleForHeapStart().ptr;
        return true;
    }

    void DescriptorHeap::Destroy()
    {
        m_Handle->Release();
    }

    ID3D12DescriptorHeap* DescriptorHeap::GetHandle() const
    {
        return m_Handle;
    }

    DescriptorHeapType DescriptorHeap::GetHeapType() const
    {
        return m_HeapType;
    }

    uint32_t DescriptorHeap::GetNumDescriptors() const
    {
        return m_NumDescriptors;
    }

    bool DescriptorHeap::IsShaderVisible() const
    {
        return m_ShaderVisible;
    }

    uintptr_t DescriptorHeap::GetCpuStart() const
    {
        return m_CpuStart;
    }

    uintptr_t DescriptorHeap::GetGpuStart() const
    {
        return m_GpuStart;
    }

    DescriptorHandle DescriptorHeap::AllocateDescriptor()
    {
        if (!m_Device) return DescriptorHandle(nullptr);
        if (!m_CpuStart || !m_GpuStart) return DescriptorHandle(nullptr);
        if (!m_Handle) return DescriptorHandle(nullptr);

        uint32_t index;

        if (!m_FreeList.IsEmpty())
        {
            index = m_FreeList.Last();
            m_FreeList.PopBack();
        }
        else
        {
            index = m_NextFree++;
            assert(index < m_NumDescriptors);
        }


        ID3D12Device13* deviceHandle = m_Device->GetHandle();
        const uint32_t descriptorSize = deviceHandle->GetDescriptorHandleIncrementSize(D3D12HeapType(m_HeapType));

        DescriptorHandle handle;
        handle.index = index;
        handle.cpu = m_CpuStart + index * descriptorSize;
        handle.gpu = m_GpuStart + index * descriptorSize;
        return handle;
    }

    void DescriptorHeap::FreeDescriptor(DescriptorHandle descriptor)
    {

    }
}
