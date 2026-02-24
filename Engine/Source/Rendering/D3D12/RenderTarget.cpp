#include "RenderTarget.h"
#include "Device.h"

namespace Nova::D3D12
{
    bool RenderTarget::Initialize(const RenderTargetCreateInfo& createInfo)
    {
        return false;
    }

    void RenderTarget::Destroy()
    {
    }

    bool RenderTarget::Resize(uint32_t newX, uint32_t newY)
    {
        return false;
    }

    const Texture& RenderTarget::GetColorTexture()
    {
        const size_t imageIndex = ((Device*)m_Device)->GetCurrentFrameIndex();
        const auto createTexture = [this, &imageIndex]() -> Texture
        {
            Texture texture;
            texture.m_Width = m_Width;
            texture.m_Height = m_Height;
            texture.m_Image = m_ColorImages[imageIndex];
            texture.m_ImageView = m_ColorImageViews[imageIndex];
            texture.m_SampleCount = m_SampleCount;
            texture.m_Device = (Device*)m_Device;
            texture.m_Format = m_ColorFormat;
            texture.m_Allocation = m_ColorAllocations[imageIndex];
            texture.m_Mips = 1;
            texture.m_State = ResourceState::ColorAttachment;
            texture.m_SampleCount = m_SampleCount;
            return texture;
        };

        return m_ColorTexture.Get(createTexture);
    }

    const Texture& RenderTarget::GetDepthTexture()
    {
        const size_t imageIndex = ((Device*)m_Device)->GetCurrentFrameIndex();
        const auto createTexture = [this, &imageIndex]() -> Texture
        {
            Texture texture;
            texture.m_Width = m_Width;
            texture.m_Height = m_Height;
            texture.m_Image = m_DepthImages[imageIndex];
            texture.m_ImageView = m_DepthImageViews[imageIndex];
            texture.m_SampleCount = m_SampleCount;
            texture.m_Device = (Device*)m_Device;
            texture.m_Format = m_DepthFormat;
            texture.m_Allocation = m_DepthAllocations[imageIndex];
            texture.m_Mips = 1;
            texture.m_State = ResourceState::DepthStencilAttachment;
            texture.m_SampleCount = m_SampleCount;
            return texture;
        };

        return m_DepthTexture.Get(createTexture);
    }
}
