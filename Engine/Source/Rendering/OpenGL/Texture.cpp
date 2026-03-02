#include "Texture.h"
#include "RenderDevice.h"
#include <glad/glad.h>

#include "Conversions.h"

namespace Nova::OpenGL
{
    bool Texture::Initialize(const TextureCreateInfo& createInfo)
    {
        using namespace Nova;

        if (createInfo.width == 0 || createInfo.height == 0)
            return false;

        if (createInfo.usageFlags == TextureUsageFlagBits::None)
            return false;

        /*if (createInfo.data == nullptr && createInfo.dataSize == 0 && !createInfo.usageFlags.Contains(TextureUsageFlagBits::Storage))
            return false;*/

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        if (!device) return false;

        GLenum textureType = GL_TEXTURE_1D;
        if (createInfo.height > 1) textureType = GL_TEXTURE_2D;
        if (createInfo.depth > 1) textureType = GL_TEXTURE_3D;

        if (createInfo.sampleCount > 1 && textureType == GL_TEXTURE_2D)
            textureType = GL_TEXTURE_2D_MULTISAMPLE;

        if (HandleIsValid(m_Handle))
            glDeleteTextures(1, &m_Handle);

        glCreateTextures(textureType, 1, &m_Handle);
        const GLformat format = Convert<GLformat>(createInfo.format);
        switch (textureType)
        {
        case GL_TEXTURE_1D:
            glTextureStorage1D(m_Handle, createInfo.mips, format.internalFormat, createInfo.width);
            break;
        case GL_TEXTURE_2D:
            glTextureStorage2D(m_Handle, createInfo.mips, format.internalFormat, createInfo.width, createInfo.height);
            break;
        case GL_TEXTURE_2D_MULTISAMPLE:
            glTextureStorage2DMultisample(m_Handle, createInfo.sampleCount, format.internalFormat, createInfo.width, createInfo.height, true);
            break;
        case GL_TEXTURE_3D:
            glTextureStorage3D(m_Handle, createInfo.mips, format.internalFormat, createInfo.width, createInfo.height, createInfo.depth);
            break;
        default: break;
        }

        if (createInfo.data && createInfo.dataSize > 0)
        {
            switch (textureType)
            {
            case GL_TEXTURE_1D:
                glTextureSubImage1D(m_Handle, 0, 0, createInfo.width, format.format, GL_UNSIGNED_BYTE, createInfo.data);
                break;
            case GL_TEXTURE_2D:
                glTextureSubImage2D(m_Handle, 0, 0, 0, createInfo.width, createInfo.height, format.format, GL_UNSIGNED_BYTE, createInfo.data);
                break;
            case GL_TEXTURE_2D_MULTISAMPLE:
                glTextureSubImage2D(m_Handle, 0, 0, 0, createInfo.width, createInfo.height, format.format, GL_UNSIGNED_BYTE, createInfo.data);
                break;
            case GL_TEXTURE_3D:
                glTextureSubImage3D(m_Handle, 0, 0, 0, 0, createInfo.width, createInfo.height, createInfo.depth, format.format, GL_UNSIGNED_BYTE, createInfo.data);
                break;
            default: break;
            }
        }

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
        glDeleteTextures(1, &m_Handle);
        m_Handle = INVALID_HANDLE<uint32_t>;
    }

    bool Texture::IsValid()
    {
        return m_Device && m_Handle != 0xFFFFFFFF;
    }

    uint32_t Texture::GetHandle() const
    {
        return m_Handle;
    }

    Array<uint8_t> Texture::GetPixels()
    {
        return {};
    }
}
