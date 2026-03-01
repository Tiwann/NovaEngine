#include "Swapchain.h"
#include "Conversions.h"
#include "Format.h"
#include "Rendering/Surface.h"
#include <glad/glad.h>


namespace Nova::OpenGL
{
    bool Swapchain::Initialize(const SwapchainCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.surface) return false;
        if (createInfo.width <= 0 || createInfo.height <= 0) return false;
        if (!(createInfo.presentMode == PresentMode::Fifo || createInfo.presentMode == PresentMode::Immediate))
            return false;

        const uint32_t imageCount = (uint32_t)createInfo.buffering;
        glDeleteTextures(imageCount, m_Textures);

        glCreateTextures(GL_TEXTURE_2D, imageCount, m_Textures);
        for (uint32_t i = 0; i < imageCount; ++i)
        {
            const GLformat format = Convert<GLformat>(createInfo.format);
            glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
            glTextureStorage2D(m_Textures[i], 1, format.internalFormat, createInfo.width, createInfo.height);
        }

        m_Device = createInfo.device;
        m_Buffering = createInfo.buffering;
        m_HasVSync = createInfo.presentMode == PresentMode::Fifo;
        m_ImageFormat = createInfo.format;
        m_ImagePresentMode = createInfo.presentMode;
        m_ImageWidth = createInfo.width;
        m_ImageHeight = createInfo.height;
        m_Surface = createInfo.surface;
        m_Valid = true;
        return true;
    }

    void Swapchain::Destroy()
    {
        const uint32_t imageCount = (uint32_t)m_Buffering;
        glDeleteTextures(imageCount, m_Textures);
    }

    bool Swapchain::Recreate()
    {
        const Nova::Surface* surface = GetSurface();
        if (!surface) return false;

        SwapchainCreateInfo createInfo;
        createInfo.device = m_Device;
        createInfo.surface = m_Surface;
        createInfo.width = surface->GetWidth();
        createInfo.height = surface->GetHeight();
        createInfo.format = m_ImageFormat;
        createInfo.buffering = m_Buffering;
        createInfo.presentMode = m_ImagePresentMode;
        createInfo.recycle = true;
        return Initialize(createInfo);
    }

    void Swapchain::SetName(StringView name)
    {
    }

    Ref<Nova::Texture> Swapchain::GetTexture(uint32_t index)
    {
        return nullptr;
    }

    Ref<Nova::Texture> Swapchain::GetCurrentTexture()
    {
        return nullptr;
    }

    bool Swapchain::AcquireNextImage(uint32_t& nextImage)
    {
        static bool firstFrame = true;
        if (firstFrame)
        {
            nextImage = 0;
            firstFrame = false;
            return true;
        }

        nextImage = (nextImage + 1) % GetImageCount();
        return true;
    }
}
