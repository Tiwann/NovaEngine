#include "RenderTarget.h"
#include "RenderDevice.h"
#include "Runtime/Log.h"
#include "Rendering/Swapchain.h"
#include "Conversions.h"
#include <glad/glad.h>


namespace Nova::OpenGL
{
    bool RenderTarget::Initialize(const RenderTargetCreateInfo& createInfo)
    {
        if (!createInfo.device)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create render target: invalid render device.");
            return false;
        }

        if (createInfo.width == 0 || createInfo.height == 0 || createInfo.depth != 1)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create render target: invalid dimensions.");
            return false;
        }

        if (createInfo.sampleCount > 16)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create render target: invalid sample count.");
            return false;
        }

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        Swapchain* swapchain = static_cast<Swapchain*>(device->GetSwapchain());
        if (!swapchain)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create render target: invalid swapchain.");
            return false;
        }

        const uint32_t imageCount = swapchain->GetImageCount();
        glDeleteFramebuffers(imageCount, m_FrameBuffers);
        glDeleteTextures(imageCount, m_ColorTextures);
        glDeleteTextures(imageCount, m_DepthTextures);

        glCreateFramebuffers(imageCount, m_FrameBuffers);
        glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, imageCount, m_ColorTextures);
        glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, imageCount, m_DepthTextures);

        for (uint32_t i = 0; i < imageCount; i++)
        {
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorTextures[i]);
            const GLFormat colorFormat = Convert<GLFormat>(createInfo.colorFormat);
            glTextureStorage2DMultisample(m_ColorTextures[i], createInfo.sampleCount, colorFormat.internalFormat, createInfo.width, createInfo.height, true);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_DepthTextures[i]);
            const GLFormat depthFormat = Convert<GLFormat>(createInfo.depthFormat);
            glTextureStorage2DMultisample(m_DepthTextures[i], createInfo.sampleCount, depthFormat.internalFormat, createInfo.width, createInfo.height, true);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffers[i]);
            glNamedFramebufferTexture(m_FrameBuffers[i], GL_COLOR_ATTACHMENT0, m_ColorTextures[i], 0);
            glNamedFramebufferTexture(m_FrameBuffers[i], GL_DEPTH_STENCIL_ATTACHMENT, m_DepthTextures[i], 0);

            const GLenum status = glCheckNamedFramebufferStatus(m_FrameBuffers[i], GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to initialize framebuffer: {}", i);
                return false;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        m_Device = device;
        m_Width = createInfo.width;
        m_Height = createInfo.height;
        m_Depth = createInfo.depth;
        m_ColorFormat = createInfo.colorFormat;
        m_DepthFormat = createInfo.depthFormat;
        m_SampleCount = createInfo.sampleCount;
        m_ImageCount = imageCount;
        return true;
    }

    void RenderTarget::Destroy()
    {
        glDeleteFramebuffers(m_ImageCount, m_FrameBuffers);
        glDeleteTextures(m_ImageCount, m_ColorTextures);
        glDeleteTextures(m_ImageCount, m_DepthTextures);

        Memory::Memset(m_FrameBuffers, 0xFFFFFFFF, m_ImageCount);
        Memory::Memset(m_ColorTextures, 0xFFFFFFFF, m_ImageCount);
        Memory::Memset(m_DepthTextures, 0xFFFFFFFF, m_ImageCount);
    }

    bool RenderTarget::Resize(const uint32_t newX, const uint32_t newY)
    {
        if (newX == 0 || newY == 0) return false;
        Destroy();
        RenderTargetCreateInfo createInfo;
        createInfo.device = m_Device;
        createInfo.width = newX;
        createInfo.height = newY;
        createInfo.depth = m_Depth;
        createInfo.sampleCount = m_SampleCount;
        createInfo.colorFormat = m_ColorFormat;
        createInfo.depthFormat = m_DepthFormat;
        return Initialize(createInfo);
    }

    Nova::Ref<Nova::Texture> RenderTarget::GetColorTexture()
    {
        return nullptr;
    }

    Nova::Ref<Nova::Texture> RenderTarget::GetDepthTexture()
    {
        return nullptr;
    }
}
