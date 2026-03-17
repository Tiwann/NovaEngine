#include "TextureView.h"
#include "RenderDevice.h"
#include <directx/d3dx12.h>

#include "Conversions.h"

namespace Nova::D3D12
{
    static D3D12_SRV_DIMENSION GetDimension(TextureDimension dimension, uint32_t sampleCount, uint32_t arrayCount)
    {
        switch (dimension)
        {
        case TextureDimension::None: return D3D12_SRV_DIMENSION_UNKNOWN;
        case TextureDimension::Dim1D:
            return arrayCount > 1 ? D3D12_SRV_DIMENSION_TEXTURE1DARRAY : D3D12_SRV_DIMENSION_TEXTURE1D;
        case TextureDimension::Dim2D:
            {
                if (arrayCount > 1 && sampleCount > 1)
                    return D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
                if (arrayCount > 1)
                    return D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                if (sampleCount > 1)
                    return D3D12_SRV_DIMENSION_TEXTURE2DMS;
                return D3D12_SRV_DIMENSION_TEXTURE2D;
            }
            break;
        case TextureDimension::Dim3D:
            return D3D12_SRV_DIMENSION_TEXTURE3D;
        default: return D3D12_SRV_DIMENSION_UNKNOWN;
        }
    }

    bool TextureView::Initialize(const TextureViewCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.texture) return false;

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        const Texture* texture = static_cast<const Texture*>(createInfo.texture);
        ID3D12Device13* deviceHandle = device->GetHandle();

        switch (texture->GetUsageFlags())
        {
        case (uint32_t)TextureUsageFlagBits::ColorAttachment:
            {

                D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
                srvDesc.Format = Convert<DXGI_FORMAT>(createInfo.format);
                srvDesc.ViewDimension = GetDimension(texture->GetDimension(), texture->GetSampleCount(), createInfo.arrayCount);
                srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
                switch (texture->GetDimension())
                {
                case TextureDimension::None:
                    break;
                case TextureDimension::Dim1D:
                    {
                        if (createInfo.arrayCount > 1)
                        {
                            srvDesc.Texture1DArray.ArraySize = createInfo.arrayCount;
                            srvDesc.Texture1DArray.FirstArraySlice = createInfo.baseArray;
                            srvDesc.Texture1DArray.MostDetailedMip = createInfo.baseMipLevel;
                            srvDesc.Texture1DArray.MipLevels = createInfo.mipCount;
                            srvDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
                        } else
                        {
                            srvDesc.Texture1D.MostDetailedMip = createInfo.baseMipLevel;
                            srvDesc.Texture1D.MipLevels = createInfo.mipCount;
                            srvDesc.Texture1D.ResourceMinLODClamp = 0.0f;
                        }
                    }
                    break;
                case TextureDimension::Dim2D:
                    {
                        if (createInfo.arrayCount > 1)
                        {
                            if (texture->GetSampleCount() > 1)
                            {
                                srvDesc.Texture2DMSArray.ArraySize = createInfo.arrayCount;
                                srvDesc.Texture2DMSArray.FirstArraySlice = createInfo.baseArray;
                            } else
                            {
                                srvDesc.Texture2DArray.ArraySize = createInfo.arrayCount;
                                srvDesc.Texture2DArray.FirstArraySlice = createInfo.baseArray;
                                srvDesc.Texture2DArray.MostDetailedMip = createInfo.baseMipLevel;
                                srvDesc.Texture2DArray.MipLevels = createInfo.mipCount;
                                srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
                                srvDesc.Texture2DArray.PlaneSlice = 0; // ONLY FOR YUV FORMATS
                            }
                        } else
                        {
                            if (texture->GetSampleCount() > 1)
                            {
                                //srvDesc.Texture2DMS
                            } else
                            {
                                srvDesc.Texture2D.MostDetailedMip = createInfo.baseMipLevel;
                                srvDesc.Texture2D.MipLevels = createInfo.mipCount;
                                srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
                                srvDesc.Texture2D.PlaneSlice = 0;
                            }
                        }
                    }
                    break;
                case TextureDimension::Dim3D:
                    {
                        srvDesc.Texture3D.MostDetailedMip = createInfo.baseMipLevel;
                        srvDesc.Texture3D.MipLevels = createInfo.mipCount;
                        srvDesc.Texture3D.ResourceMinLODClamp = 0.0f;
                    }
                    break;
                }

                //deviceHandle->CreateShaderResourceView(texture->GetImage(), &srvDesc, descriptorHandle);
            }
            break;
        case (uint32_t)TextureUsageFlagBits::DepthStencilAttachment:
            {
                D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
                //dsvDesc.
            }
            break;
        case (uint32_t)TextureUsageFlagBits::Storage:
            {
                D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            }
            break;
        case (uint32_t)TextureUsageFlagBits::Sampled:
            {

            }
        }

        m_Device = createInfo.device;
        m_Texture = createInfo.texture;
        m_Format = createInfo.format;
        m_AspectFlags = createInfo.aspectFlags;
        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_Depth = createInfo.depth;
        m_BaseMipLevel = createInfo.baseMipLevel;
        m_MipCount = createInfo.mipCount;
        return true;
    }

    void TextureView::Destroy()
    {
        RenderDevice* device = static_cast<RenderDevice*>(m_Device);
        ID3D12Device13* deviceHandle = device->GetHandle();

    }
}
