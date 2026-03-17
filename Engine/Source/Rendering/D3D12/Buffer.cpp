#include "Buffer.h"
#include "RenderDevice.h"
#include <D3D12MemAlloc.h>
#include <directx/d3d12.h>


using namespace D3D12MA;

namespace Nova::D3D12
{
    bool Buffer::Initialize(const BufferCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (createInfo.size == 0) return false;

        const D3D12_HEAP_TYPE heapType = createInfo.usage == BufferUsage::StagingBuffer
            ? D3D12_HEAP_TYPE_UPLOAD : D3D12_HEAP_TYPE_DEFAULT;
        const CALLOCATION_DESC allocDesc = CALLOCATION_DESC{ heapType };
        
        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = createInfo.size;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        RenderDevice* device = (RenderDevice*)createInfo.device;
        ID3D12Allocator* allocator = device->GetAllocator();
        if (!allocator) return false;

        ID3D12Resource* buffer = nullptr;
        ID3D12Allocation* allocation = nullptr;

        D3D12_RESOURCE_STATES initialState =
            (heapType == D3D12_HEAP_TYPE_UPLOAD)
                ? D3D12_RESOURCE_STATE_GENERIC_READ
                : D3D12_RESOURCE_STATE_COMMON;

        if (DX_FAILED(allocator->CreateResource(
            &allocDesc,
            &resourceDesc,
            initialState,
            nullptr,
            &allocation,
            IID_PPV_ARGS(&buffer))))
        return false;

        if (m_Handle) m_Handle->Release();
        if (m_Allocation) m_Allocation->Release();
        m_Allocation = allocation;
        m_Handle = buffer;
        m_Device = device;
        m_Size = createInfo.size;
        m_State = initialState == D3D12_RESOURCE_STATE_GENERIC_READ ? ResourceState::TransferSource : ResourceState::General;
        return true;
    }

    void Buffer::Destroy()
    {
        if (m_Handle)
        {
            m_Handle->Release();
            m_Handle = nullptr;
        }

        if (m_Allocation)
        {
            m_Allocation->Release();
            m_Allocation = nullptr;
        }
    }

    void* Buffer::Map()
    {
        void* mapped = nullptr;
        const D3D12_RANGE& range = D3D12_RANGE(0, 0);
        if (DX_FAILED(m_Handle->Map(0, &range, &mapped)))
            return nullptr;
        return mapped;
    }

    void Buffer::Unmap(const void* ptr)
    {
        (void)ptr;
        const D3D12_RANGE& range = D3D12_RANGE(0, m_Size);
        m_Handle->Unmap(0, &range);
    }

    ID3D12Resource* Buffer::GetHandle() const
    {
        return m_Handle;
    }
}
