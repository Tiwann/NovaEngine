#include "OpenGLTexture2D.h"
#include "Runtime/Log.h"
#include "Containers/StringFormat.h"
#include "Runtime/Format.h"
#include "OpenGLRenderer.h"
#include <glad/gl.h>


namespace Nova
{
    OpenGLTexture2D::OpenGLTexture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot) : Texture2D(Name, Width, Height, Params, Slot)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
        glActiveTexture(GL_TEXTURE0 + Slot);
        glBindTexture(GL_TEXTURE_2D, m_Handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLRenderer::ConvertFilter(m_Params.Filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLRenderer::ConvertFilter(m_Params.Filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLRenderer::ConvertSamplerAddressMode(m_Params.AddressMode));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLRenderer::ConvertSamplerAddressMode(m_Params.AddressMode));

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_Width, (GLsizei)m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_Handle);
    }

    void OpenGLTexture2D::SetTextureParameters(const TextureParams& Params)
    {
        Bind();
        m_Params = Params;
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, OpenGLRenderer::ConvertFilter(m_Params.Filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, OpenGLRenderer::ConvertFilter(m_Params.Filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, OpenGLRenderer::ConvertSamplerAddressMode(m_Params.AddressMode));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, OpenGLRenderer::ConvertSamplerAddressMode(m_Params.AddressMode));
        Unbind();
    }

    void OpenGLTexture2D::SetData(u8* Data, u32 Width, u32 Height, const Format& Format)
    {
        Bind();
        m_Width = Width;
        m_Height = Height;
        m_Params.Format = Format;
        const GLenum Type = OpenGLRenderer::GetFormatType(m_Params.Format);
        const GLenum Fmt = OpenGLRenderer::ConvertFormat(m_Params.Format);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_Width, (GLsizei)m_Height, 0, Fmt, Type, Data);
        Unbind();
    }

    void OpenGLTexture2D::Bind() const
    {
        glActiveTexture(GL_TEXTURE0 + m_Slot);
        glBindTexture(GL_TEXTURE_2D, m_Handle);
    }

    void OpenGLTexture2D::Unbind() const
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    uintptr_t OpenGLTexture2D::GetHandle() const
    {
        return m_Handle;
    }
}
