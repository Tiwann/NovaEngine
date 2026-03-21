#include "Sampler.h"
#include "Conversions.h"
#include "RenderDevice.h"
#include <directx/d3dx12.h>


namespace Nova::D3D12
{
    bool Sampler::Initialize(const SamplerCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        D3D12_SAMPLER_DESC2 desc = {};
        desc.AddressU = Convert<D3D12_TEXTURE_ADDRESS_MODE>(createInfo.addressModeU);
        desc.AddressV = Convert<D3D12_TEXTURE_ADDRESS_MODE>(createInfo.addressModeV);
        desc.AddressW = Convert<D3D12_TEXTURE_ADDRESS_MODE>(createInfo.addressModeW);
        desc.ComparisonFunc = Convert<D3D12_COMPARISON_FUNC>(createInfo.compareOp);
        desc.Filter = Convert<D3D12_FILTER>(createInfo.magFilter);
        desc.MaxAnisotropy = createInfo.anisotropyEnable ? 1 : 0;
        desc.MinLOD = createInfo.minLod;
        desc.MaxLOD = createInfo.maxLod;
        if (createInfo.unnormalizedCoordinates)
            desc.Flags |= D3D12_SAMPLER_FLAG_NON_NORMALIZED_COORDINATES;

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        DescriptorHeap* samplerHeap = device->GetDescriptorHeap();
        m_Handle = samplerHeap->AllocateDescriptor();
        m_Device = device;
        return false;
    }

    void Sampler::Destroy()
    {
        DescriptorHeap* samplerHeap = m_Device->GetDescriptorHeap();
        samplerHeap->FreeDescriptor(m_Handle);
    }
}
