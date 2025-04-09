#include "OpenGLTexture2D.h"
#include "Runtime/Log.h"
#include "Runtime/LogVerbosity.h"
#include "Runtime/Memory.h"
#include "Containers/StringFormat.h"
#include <glad/gl.h>

namespace Nova
{
    OpenGLTexture2D::OpenGLTexture2D(const String& Name, u32 Width, u32 Height, const TextureParams& Params, u32 Slot) : Texture2D(Name, Width, Height, Params, Slot)
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);
        glActiveTexture(GL_TEXTURE0 + Slot);
        glBindTexture(GL_TEXTURE_2D, m_Handle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilter(m_Params.Filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilter(m_Params.Filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetTextureWrap(m_Params.Wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetTextureWrap(m_Params.Wrap));

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
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetTextureFilter(m_Params.Filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetTextureFilter(m_Params.Filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetTextureWrap(m_Params.Wrap));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetTextureWrap(m_Params.Wrap));
        Unbind();
    }

    void OpenGLTexture2D::SetData(u8* Data, u32 Width, u32 Height, Format Format)
    {
        Bind();
        m_Width = Width;
        m_Height = Height;
        m_Format = Format;
        const GLenum Type = FormatToType(m_Format);
        const GLenum Fmt = FormatToOpenGLFormat(m_Format);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)m_Width, (GLsizei)m_Height, 0, Fmt, Type, Data);
        Unbind();
    }

    SharedPtr<Image> OpenGLTexture2D::GetImage() const
    {
        Bind();
        size_t Size = 0;
        switch (m_Format)
        {
            case Format::RGBA8: Size = m_Width * m_Height * 4; break;
            case Format::RGBA16: Size = m_Width * m_Height * 4 * 2; break;
            case Format::RGBA32F: Size = m_Width * m_Height * 4 * 4; break;
        }
        u8* Data = (u8*)NOVA_MALLOC(Size);
        glGetTextureImage(m_Handle, 0, GL_RGBA, FormatToType(m_Format), (GLsizei)Size, Data);
        SharedPtr<Image> ImageData = CreateRef<Image>(m_Width, m_Height, m_Format, Data);
        NOVA_FREE(Data);
        Unbind();
        return ImageData;
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

    bool OpenGLTexture2D::GetPixels(Buffer<u8>& OutPixels) const
    {
        Bind();
        const size_t BytePerPixelChannel = m_Format == Format::RGBA8 ? 1ULL : m_Format == Format::RGBA16 ? 2ULL : 4ULL;
        const size_t Size = (size_t)m_Width * (size_t)m_Height * 4ULL * BytePerPixelChannel;
        OutPixels.Allocate(Size);
        glGetTextureImage(m_Handle, 0, FormatToOpenGLFormat(m_Format), FormatToType(m_Format), (GLsizei)Size, (GLvoid*)OutPixels.Data());
        return !OutPixels.IsEmpty();
    }
    

    u32 OpenGLTexture2D::FormatToType(Format Format) const
    {
        switch (Format) {
        case Format::RGBA8: return GL_UNSIGNED_BYTE;
        case Format::RGBA16: return GL_UNSIGNED_SHORT;
        case Format::RGBA32F: return GL_FLOAT;
        }
        return 0;
    }

    GLint OpenGLTexture2D::GetTextureWrap(TextureWrap Wrap)
    {
        switch (Wrap) {
        case TextureWrap::Clamp: return GL_CLAMP_TO_EDGE;
        case TextureWrap::Repeat: return GL_REPEAT;
        case TextureWrap::Mirror: return GL_MIRRORED_REPEAT;
        }
        return 0;
    }

    GLint OpenGLTexture2D::GetTextureFilter(TextureFilter Filter)
    {
        switch (Filter)
        {
        case TextureFilter::Nearest: return GL_NEAREST;
        case TextureFilter::Linear: return GL_LINEAR;
        }
        return 0;
    }

    u32 OpenGLTexture2D::FormatToOpenGLFormat(Format Format) const
    {
        switch (Format) {
        case Format::RGBA8: return GL_RGBA;
        case Format::RGBA16: return GL_RGBA;
        case Format::RGBA32F: return GL_RGBA;
        }
        return 0;
    }
}
