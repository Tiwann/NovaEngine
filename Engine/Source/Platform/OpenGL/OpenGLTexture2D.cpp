#include "OpenGLTexture2D.h"
#include "Runtime/Log.h"
#include "Runtime/LogVerbosity.h"
#include "Runtime/Memory.h"
#include "Containers/StringFormat.h"
#include "Runtime/Formats.h"
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

    void OpenGLTexture2D::SetData(u8* Data, u32 Width, u32 Height, const Formats& Format)
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
            case Formats::R8G8B8A8_UNORM: Size = m_Width * m_Height * 4; break;
            case Formats::R16G16B16A16_USHORT: Size = m_Width * m_Height * 4 * 2; break;
            case Formats::R32G32B32A32_FLOAT: Size = m_Width * m_Height * 4 * 4; break;
        }
        u8* Data = Memory::Malloc<u8>(Size);
        glGetTextureImage(m_Handle, 0, GL_RGBA, FormatToType(m_Format), (GLsizei)Size, Data);
        SharedPtr<Image> ImageData = MakeShared<Image>(m_Width, m_Height, m_Format, Data);
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


    u32 OpenGLTexture2D::FormatToType(Formats Format) const
    {
        switch (Format)
        {
        case Formats::NONE: return 0;
        case Formats::R8_UNORM: return GL_UNSIGNED_BYTE;
        case Formats::R8_SNORM: return GL_BYTE;
        case Formats::R16_USHORT: return GL_UNSIGNED_SHORT;
        case Formats::R16_SHORT: return GL_SHORT;
        case Formats::R32_FLOAT: return GL_FLOAT;
        case Formats::R32_UINT: return GL_UNSIGNED_INT;
        case Formats::R32_SINT: return GL_INT;
        case Formats::R8G8_UNORM: return GL_UNSIGNED_BYTE;
        case Formats::R8G8_SNORM: return GL_BYTE;
        case Formats::R16G16_USHORT: return GL_UNSIGNED_SHORT;
        case Formats::R16G16_SHORT: return GL_SHORT;
        case Formats::R32G32_UINT: return GL_UNSIGNED_INT;
        case Formats::R32G32_SINT: return GL_INT;
        case Formats::R32G32_FLOAT: return GL_FLOAT;
        case Formats::R8G8B8_UNORM: return GL_UNSIGNED_BYTE;
        case Formats::R8G8B8_SNORM: return GL_BYTE;
        case Formats::R16G16B16_USHORT: return GL_UNSIGNED_SHORT;
        case Formats::R16G16B16_SHORT: return GL_SHORT;
        case Formats::R32G32B32_UINT: return GL_UNSIGNED_INT;
        case Formats::R32G32B32_SINT: return GL_INT;
        case Formats::R32G32B32_FLOAT: return GL_FLOAT;
        case Formats::R8G8B8A8_UNORM: return GL_UNSIGNED_BYTE;
        case Formats::R8G8B8A8_SNORM: return GL_BYTE;
        case Formats::R16G16B16A16_USHORT: return GL_UNSIGNED_SHORT;
        case Formats::R16G16B16A16_SHORT: return GL_SHORT;
        case Formats::R32G32B32A32_UINT: return GL_UNSIGNED_INT;
        case Formats::R32G32B32A32_SINT: return GL_INT;
        case Formats::R32G32B32A32_FLOAT: return GL_FLOAT;
        default: return 0;
        }
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

    u32 OpenGLTexture2D::FormatToOpenGLFormat(Formats Format) const
    {
        switch (Format)
        {
        case Formats::NONE: return GL_NONE;
        case Formats::R8_UNORM: return GL_R8;
        case Formats::R8_SNORM: return GL_R8;
        case Formats::R16_USHORT: return GL_R16;
        case Formats::R16_SHORT: return GL_R16;
        case Formats::R32_FLOAT: return GL_R32F;
        case Formats::R32_UINT: return GL_R32UI;
        case Formats::R32_SINT: return GL_R32I;
        case Formats::R8G8_UNORM: return GL_RG8;
        case Formats::R8G8_SNORM:
            break;
        case Formats::R16G16_USHORT:
            break;
        case Formats::R16G16_SHORT:
            break;
        case Formats::R32G32_UINT:
            break;
        case Formats::R32G32_SINT:
            break;
        case Formats::R32G32_FLOAT:
            break;
        case Formats::R8G8B8_UNORM:
            break;
        case Formats::R8G8B8_SNORM:
            break;
        case Formats::R16G16B16_USHORT:
            break;
        case Formats::R16G16B16_SHORT:
            break;
        case Formats::R32G32B32_UINT:
            break;
        case Formats::R32G32B32_SINT:
            break;
        case Formats::R32G32B32_FLOAT:
            break;
        case Formats::R8G8B8A8_UNORM:
            break;
        case Formats::R8G8B8A8_SNORM:
            break;
        case Formats::R16G16B16A16_USHORT:
            break;
        case Formats::R16G16B16A16_SHORT:
            break;
        case Formats::R32G32B32A32_UINT:
            break;
        case Formats::R32G32B32A32_SINT:
            break;
        case Formats::R32G32B32A32_FLOAT:
            break;
        }
        return 0;
    }
}
