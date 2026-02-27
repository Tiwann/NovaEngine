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

        if (DX_FAILED(allocator->CreateResource(
            &allocDesc,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            &allocation,
            IID_PPV_ARGS(&buffer))))
        return false;

        if (m_Handle) m_Handle->Release();
        if (m_Allocation) m_Allocation->Release();
        m_Allocation = allocation;
        m_Handle = buffer;
        m_Device = device;
        return true;
    }

    void Buffer::Destroy()
    {
        if (m_Handle) m_Handle->Release();
        if (m_Allocation) m_Allocation->Release();
    }

    bool Buffer::Resize(size_t newSize, bool keepData)
    {
        return false;
    }

    bool Buffer::CPUCopy(const void* src, size_t offset, size_t size)
    {
        if (!m_Handle) return false;

        uint8_t* mapped = nullptr;
        if (DX_FAILED(m_Handle->Map(0, nullptr, (void**)&mapped)))
            return false;
        Memory::Memcpy(mapped + offset, src, size);
        m_Handle->Unmap(0, nullptr);
        return true;
    }

    bool Buffer::CPUCopy(size_t offset, size_t size, void* outBuffer)
    {
        if (!m_Handle) return false;
        uint8_t* mapped = nullptr;
        if (DX_FAILED(m_Handle->Map(0, nullptr, (void**)&mapped)))
            return false;
        Memory::Memcpy((uint8_t*)outBuffer + offset, mapped, size);
        m_Handle->Unmap(0, nullptr);
        return true;
    }

    bool Buffer::GPUCopy(Nova::Buffer& other, size_t srcOffset, size_t destOffset, size_t size)
    {
        return false;
    }

    void Buffer::Memset(size_t value, size_t size)
    {
        if (!m_Handle) return;
        uint8_t* mapped = nullptr;
        if (DX_FAILED(m_Handle->Map(0, nullptr, (void**)&mapped)))
            return;
        Memory::Memset(mapped, 0, size);
        m_Handle->Unmap(0, nullptr);
    }
}
