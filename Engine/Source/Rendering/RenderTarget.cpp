#include "RenderDevice.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "TextureView.h"

namespace Nova
{
    bool RenderTarget::Initialize(const RenderTargetCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (createInfo.width == 0) return false;
        if (createInfo.height == 0) return false;
        if (IsDepthFormat(createInfo.colorFormat)) return false;
        if (!IsDepthFormat(createInfo.depthFormat)) return false;

        RenderDevice* device = createInfo.device;
        const uint32_t imageCount = device->GetImageCount();
        NOVA_ASSERT(imageCount <= 3, "Image count should be less or equal to 3!");

        const auto initializeTexture = [&device](Ref<Texture>& texture, const TextureCreateInfo& createInfo) -> bool
        {
            if (texture) {
                if (!texture->Initialize(createInfo))
                    return false;
            } else {
                texture = device->CreateTexture(createInfo);
                if (!texture) return false;
            }

            return true;
        };

        const auto initializeTextureView = [&device](Ref<TextureView>& textureView, const TextureViewCreateInfo& createInfo) -> bool
        {
            if (textureView)
            {
                if (!textureView->Initialize(createInfo))
                    return false;
            } else
            {
                textureView = device->CreateTextureView(createInfo);
                if (!textureView) return false;
            }

            return true;
        };

        for (uint32_t i = 0; i < imageCount; i++)
        {
            TextureCreateInfo colorCreateInfo;
            colorCreateInfo.device = device;
            colorCreateInfo.format = createInfo.colorFormat;
            colorCreateInfo.usageFlags = TextureUsageFlagBits::ColorAttachment;
            colorCreateInfo.width = createInfo.width;
            colorCreateInfo.height = createInfo.height;
            colorCreateInfo.depth = 1;
            colorCreateInfo.mipCount = 1;
            colorCreateInfo.sampleCount = createInfo.sampleCount;
            if (!initializeTexture(m_ColorTextures[i], colorCreateInfo))
                return false;

            TextureCreateInfo depthCreateInfo;
            depthCreateInfo.device = device;
            depthCreateInfo.format = createInfo.depthFormat;
            depthCreateInfo.usageFlags = TextureUsageFlagBits::DepthStencilAttachment;
            depthCreateInfo.width = createInfo.width;
            depthCreateInfo.height = createInfo.height;
            depthCreateInfo.depth = 1;
            depthCreateInfo.mipCount = 1;
            depthCreateInfo.sampleCount = createInfo.sampleCount;
            if (!initializeTexture(m_DepthTextures[i], depthCreateInfo))
                return false;

            TextureViewCreateInfo colorViewCreateInfo;
            colorViewCreateInfo.device = device;
            colorViewCreateInfo.texture = m_ColorTextures[i];
            colorViewCreateInfo.format = createInfo.colorFormat;
            colorViewCreateInfo.width = createInfo.width;
            colorViewCreateInfo.height = createInfo.height;
            colorViewCreateInfo.depth = 1;
            colorViewCreateInfo.aspectFlags = TextureAspectFlagBits::Color;
            colorViewCreateInfo.baseMipLevel = 0;
            colorViewCreateInfo.mipCount = 1;
            if (!initializeTextureView(m_ColorTextureViews[i], colorViewCreateInfo))
                return false;

            TextureViewCreateInfo depthViewCreateInfo;
            depthViewCreateInfo.device = device;
            depthViewCreateInfo.texture = m_DepthTextures[i];
            depthViewCreateInfo.format = createInfo.depthFormat;
            depthViewCreateInfo.width = createInfo.width;
            depthViewCreateInfo.height = createInfo.height;
            depthViewCreateInfo.depth = 1;
            depthViewCreateInfo.aspectFlags = TextureAspectFlagBits::Depth | TextureAspectFlagBits::Stencil;
            depthViewCreateInfo.baseMipLevel = 0;
            depthViewCreateInfo.mipCount = 1;
            if (!initializeTextureView(m_DepthTextureViews[i], depthViewCreateInfo))
                return false;
        }


        m_Device = device;
        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_ColorFormat = createInfo.colorFormat;
        m_DepthFormat = createInfo.depthFormat;
        m_SampleCount = createInfo.sampleCount;
        m_ImageCount = imageCount;
        return true;
    }

    void RenderTarget::Destroy()
    {
        for (uint32_t i = 0; i < m_ImageCount; i++)
        {
            if (m_ColorTextures[i]) m_ColorTextures[i]->Destroy();
            if (m_DepthTextures[i]) m_DepthTextures[i]->Destroy();
            if (m_ColorTextureViews[i]) m_ColorTextureViews[i]->Destroy();
            if (m_DepthTextureViews[i]) m_DepthTextureViews[i]->Destroy();
        }
    }

    void RenderTarget::Resize(const uint32_t newWidth, const uint32_t newHeight)
    {
        RenderTargetCreateInfo createInfo;
        createInfo.device = m_Device;
        createInfo.width = newWidth;
        createInfo.height = newHeight;
        createInfo.sampleCount = m_SampleCount;
        createInfo.colorFormat = m_ColorFormat;
        createInfo.depthFormat = m_DepthFormat;
        createInfo.sampleCount = m_SampleCount;
        Initialize(createInfo);
    }

    Ref<Texture> RenderTarget::GetColorTexture()
    {
        const uint32_t index = m_Device->GetCurrentFrameIndex();
        return m_ColorTextures[index];
    }

    Ref<Texture> RenderTarget::GetDepthTexture()
    {
        const uint32_t index = m_Device->GetCurrentFrameIndex();
        return m_DepthTextures[index];
    }

    Ref<TextureView> RenderTarget::GetColorTextureView()
    {
        const uint32_t index = m_Device->GetCurrentFrameIndex();
        return m_ColorTextureViews[index];
    }

    Ref<TextureView> RenderTarget::GetDepthTextureView()
    {
        const uint32_t index = m_Device->GetCurrentFrameIndex();
        return m_DepthTextureViews[index];
    }

    uint32_t RenderTarget::GetWidth() const
    {
        return m_Width;
    }

    uint32_t RenderTarget::GetHeight() const
    {
        return m_Height;
    }

    Format RenderTarget::GetColorFormat() const
    {
        return m_ColorFormat;
    }

    Format RenderTarget::GetDepthFormat() const
    {
        return m_DepthFormat;
    }

    uint32_t RenderTarget::GetSampleCount() const
    {
        return m_SampleCount;
    }

    uint32_t RenderTarget::GetImageCount() const
    {
        return m_ImageCount;
    }
}
