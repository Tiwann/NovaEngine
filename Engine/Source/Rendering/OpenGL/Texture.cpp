#include "Texture.h"
#include "Device.h"
#include <glad/glad.h>

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

        Device* device = static_cast<Device*>(createInfo.device);
        if (!device) return false;

        GLenum textureType = GL_TEXTURE_1D;
        if (createInfo.height > 1) textureType = GL_TEXTURE_2D;
        if (createInfo.depth > 1) textureType = GL_TEXTURE_3D;

        if (createInfo.sampleCount > 0 && textureType == GL_TEXTURE_2D)
            textureType = GL_TEXTURE_2D_MULTISAMPLE;

        glCreateTextures(textureType, 1, &m_Handle);

        if (createInfo.data && createInfo.dataSize > 0)
        {
            switch (textureType)
            {
            case GL_TEXTURE_1D:
                {
                    glTexImage1D(textureType, 0, 0, createInfo.width, 0, GL_RGBA8, GL_UNSIGNED_BYTE, createInfo.data);
                    break;
                }
            case GL_TEXTURE_2D:
                {
                    glTexImage2D(textureType, 0, 0, createInfo.width, createInfo.height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, createInfo.data);
                    break;
                }
                case GL_TEXTURE_3D:
                {
                    glTexImage3D(textureType, 0, 0, createInfo.width, createInfo.height, createInfo.depth, 0, GL_RGBA8, GL_UNSIGNED_BYTE, createInfo.data);
                }
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
        m_Handle = 0xFFFFFFFF;
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
