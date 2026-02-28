#include "Sampler.h"
#include "Conversions.h"
#include "RenderDevice.h"
#include <glad/glad.h>


namespace Nova::OpenGL
{
    bool Sampler::Initialize(const SamplerCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);

        if (HandleIsValid(m_Handle))
            glDeleteSamplers(1, &m_Handle);

        glCreateSamplers(1, &m_Handle);

        glSamplerParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, Convert<GLenum>(createInfo.minFilter));
        glSamplerParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, Convert<GLenum>(createInfo.magFilter));

        glSamplerParameteri(m_Handle, GL_TEXTURE_WRAP_S, Convert<GLenum>(createInfo.addressModeU));
        glSamplerParameteri(m_Handle, GL_TEXTURE_WRAP_T, Convert<GLenum>(createInfo.addressModeV));
        glSamplerParameteri(m_Handle, GL_TEXTURE_WRAP_R, Convert<GLenum>(createInfo.addressModeW));

        glSamplerParameterf(m_Handle, GL_TEXTURE_MIN_LOD, createInfo.minLod);
        glSamplerParameterf(m_Handle, GL_TEXTURE_MAX_LOD, createInfo.maxLod);
        glSamplerParameterf(m_Handle, GL_TEXTURE_MAX_ANISOTROPY, createInfo.anisotropyEnable ? 1.0f : 0.0f);

        if (createInfo.compareEnable)
        {
            glSamplerParameteri(m_Handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glSamplerParameteri(m_Handle, GL_TEXTURE_COMPARE_FUNC, Convert<GLenum>(createInfo.compareOp));
        }
        else
        {
            glSamplerParameteri(m_Handle, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        }

        m_Device = device;
        m_AddressModeU = createInfo.addressModeU;
        m_AddressModeV = createInfo.addressModeV;
        m_AddressModeW = createInfo.addressModeW;
        m_MinFilter = createInfo.minFilter;
        m_MagFilter = createInfo.magFilter;
        m_AnisotropyEnable = createInfo.anisotropyEnable;
        m_CompareEnable = createInfo.compareEnable;
        m_CompareOp = createInfo.compareOp;
        m_UnnormalizedCoordinates = createInfo.unnormalizedCoordinates;
        m_MinLod = createInfo.minLod;
        m_MaxLod = createInfo.maxLod;
        m_MipmapFilter = createInfo.mipmapFilter;
        return true;
    }

    void Sampler::Destroy()
    {
        if (HandleIsValid(m_Handle))
        {
            glDeleteSamplers(1, &m_Handle);
            m_Handle = INVALID_HANDLE<uint32_t>;
        }
    }
}
