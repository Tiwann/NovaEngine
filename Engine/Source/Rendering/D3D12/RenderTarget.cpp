#include "RenderTarget.h"
#include "RenderDevice.h"
#include <directx/d3dx12.h>
#include <D3D12MemAlloc.h>

#include "Conversions.h"

typedef D3D12MA::ALLOCATION_DESC D3D12_ALLOCATION_DESC;

namespace Nova::D3D12
{
    bool RenderTarget::Initialize(const RenderTargetCreateInfo& createInfo)
    {
        RenderDevice* device = (RenderDevice*)createInfo.device;
        Swapchain* swapchain = static_cast<Swapchain*>(device->GetSwapchain());
        ID3D12Allocator* allocator = device->GetAllocator();
        ID3D12Device13* deviceHandle = device->GetHandle();

        for (size_t imageIndex = 0; imageIndex < swapchain->GetImageCount(); imageIndex++)
        {
            D3D12_ALLOCATION_DESC allocDesc = {};
            allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

            D3D12_RESOURCE_DIMENSION resourceDim = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
            if (createInfo.height > 1) resourceDim = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            if (createInfo.depth > 1) resourceDim = D3D12_RESOURCE_DIMENSION_TEXTURE3D;


        }
        return false;
    }

    void RenderTarget::Destroy()
    {
    }

    bool RenderTarget::Resize(uint32_t newX, uint32_t newY)
    {
        return false;
    }

    Ref<Nova::Texture> RenderTarget::GetColorTexture()
    {
        const size_t imageIndex = ((RenderDevice*)m_Device)->GetCurrentFrameIndex();
        const auto createTexture = [this, &imageIndex]()
        {
            Texture texture;
            texture.m_Width = m_Width;
            texture.m_Height = m_Height;
            texture.m_Image = m_ColorImages[imageIndex];
            texture.m_ImageView = m_ColorImageViews[imageIndex];
            texture.m_SampleCount = m_SampleCount;
            texture.m_Device = (RenderDevice*)m_Device;
            texture.m_Format = m_ColorFormat;
            texture.m_Allocation = m_ColorAllocations[imageIndex];
            texture.m_Mips = 1;
            texture.m_State = ResourceState::ColorAttachment;
            texture.m_SampleCount = m_SampleCount;
            return MakeRef<Texture>(texture);
        };

        return m_ColorTexture.Get(createTexture);
    }

    Ref<Nova::Texture> RenderTarget::GetDepthTexture()
    {
        const size_t imageIndex = ((RenderDevice*)m_Device)->GetCurrentFrameIndex();
        const auto createTexture = [this, &imageIndex]()
        {
            Texture texture;
            texture.m_Width = m_Width;
            texture.m_Height = m_Height;
            texture.m_Image = m_DepthImages[imageIndex];
            texture.m_ImageView = m_DepthImageViews[imageIndex];
            texture.m_SampleCount = m_SampleCount;
            texture.m_Device = (RenderDevice*)m_Device;
            texture.m_Format = m_DepthFormat;
            texture.m_Allocation = m_DepthAllocations[imageIndex];
            texture.m_Mips = 1;
            texture.m_State = ResourceState::DepthStencilAttachment;
            texture.m_SampleCount = m_SampleCount;
            return MakeRef<Texture>(texture);
        };

        return m_DepthTexture.Get(createTexture);
    }
}
