#include "Texture.h"
#include "Device.h"
#include "Conversions.h"
#include <directx/d3dx12.h>
#include <D3D12MemAlloc.h>


using namespace D3D12MA;

namespace Nova::D3D12
{
    bool Texture::Initialize(const TextureCreateInfo& createInfo)
    {
        if (createInfo.width == 0 || createInfo.height == 0)
            return false;

        if (createInfo.usageFlags == TextureUsageFlagBits::None)
            return false;

        if (!createInfo.device)
            return false;

        Device* device = static_cast<Device*>(createInfo.device);
        ID3D12Allocator* allocator = device->GetAllocator();
        ID3D12Device13* deviceHandle = device->GetHandle();

        ALLOCATION_DESC allocDesc = {};
        allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

        D3D12_RESOURCE_DIMENSION resourceDim = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        if (createInfo.height > 1) resourceDim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        if (createInfo.depth > 1) resourceDim = D3D12_RESOURCE_DIMENSION_TEXTURE3D;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Alignment = 0;
        resourceDesc.Dimension = resourceDim;
        resourceDesc.Width = createInfo.width;
        resourceDesc.Height = createInfo.height;
        resourceDesc.DepthOrArraySize = createInfo.depth;
        resourceDesc.Format = Convert<DXGI_FORMAT>(createInfo.format);
        resourceDesc.MipLevels = createInfo.mips;
        resourceDesc.SampleDesc.Count = createInfo.sampleCount;

        if (DX_FAILED(allocator->CreateResource(&allocDesc,
                                  &resourceDesc,
                                  D3D12_RESOURCE_STATE_COMMON,
                                  nullptr,
                                  &m_Allocation,
                                  IID_PPV_ARGS(&m_Image))))
                                  return false;


        m_Device = device;
        m_Format = createInfo.format;
        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_Depth = createInfo.depth;
        m_Mips = createInfo.mips;
        m_SampleCount = createInfo.sampleCount;
        m_UsageFlags = createInfo.usageFlags;
        return true;
    }

    void Texture::Destroy()
    {
        if (m_Image) m_Image->Release();
        if (m_Allocation) m_Allocation->Release();
    }

    bool Texture::IsValid()
    {
        return m_Device && m_Image && m_ImageView && m_Allocation;
    }

    const ID3D12Image* Texture::GetImage() const
    {
        return m_Image;
    }

    Array<uint8_t> Texture::GetPixels()
    {
        return {};
    }

    ID3D12Image* Texture::GetImage()
    {
        return m_Image;
    }

    ID3D12ImageView Texture::GetImageView() const
    {
        return m_ImageView;
    }
}
