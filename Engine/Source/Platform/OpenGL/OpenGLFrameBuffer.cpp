#include "OpenGLFrameBuffer.h"
#include "OpenGLTexture2D.h"
#include "Math/Vector2.h"
#include <glad/gl.h>

#include "Runtime/Application.h"

namespace Nova
{
    OpenGLFrameBuffer::OpenGLFrameBuffer()
    {
        glCreateFramebuffers(1, &m_Handle);
    }
    
    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        Destroy();
    }

    void OpenGLFrameBuffer::Destroy()
    {
        glDeleteFramebuffers(1, &m_Handle);
    }

    void OpenGLFrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Handle);
    }

    void OpenGLFrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::Resize(const Vector2& NewSize)
    {
        Bind();

        for (AttachmentTexture& Attachment : m_Attachments)
        {
            Attachment.Texture->Bind();
            if (Attachment.Attachment == FrameBufferAttachment::Color)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)NewSize.x, (GLsizei)NewSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            }

            else if (Attachment.Attachment == FrameBufferAttachment::Depth)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (GLsizei)NewSize.x, (GLsizei)NewSize.y, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
            }
        }

        Unbind();
    }

    AttachmentTexture OpenGLFrameBuffer::CreateAttachment(FrameBufferAttachment Attachment)
    {
        Bind();

        TextureParams TexParams;
        TexParams.Filter = Filter::Linear;
        TexParams.AddressMode = SamplerAddressMode::Repeat;

        Renderer* Renderer = g_Application->GetRenderer();
        Texture2D* Texture = Texture2D::Create("OpenGLFramebufferAttachment", 0, 0, TexParams, 0, Renderer->GetGraphicsApi());
        Texture->SetData(nullptr, 0, 0, Format::R8G8B8A8_UNORM);
        const GLenum BufferAttachment = GetOpenGLAttachment(Attachment);
        glFramebufferTexture2D(GL_FRAMEBUFFER, BufferAttachment, GL_TEXTURE_2D, (GLuint)Texture->GetHandle(), 0);

        AttachmentTexture AttachmentTexture;
        AttachmentTexture.Attachment = Attachment;
        AttachmentTexture.Texture = Texture;
        m_Attachments.Add(AttachmentTexture);
        return AttachmentTexture;
    }

    void OpenGLFrameBuffer::RemoveAttachment(const AttachmentTexture& Attachment)
    {
        if (!m_Attachments.Contains(Attachment)) return;
        
        Bind();
        delete Attachment.Texture;
        m_Attachments.Remove(Attachment);
    }

    GLenum OpenGLFrameBuffer::GetOpenGLAttachment(FrameBufferAttachment Attachment)
    {
        switch (Attachment)
        {
        case FrameBufferAttachment::Color:
            return GL_COLOR_ATTACHMENT0;
        case FrameBufferAttachment::Depth:
            return GL_DEPTH_ATTACHMENT;
        case FrameBufferAttachment::Stencil:
            return GL_STENCIL_ATTACHMENT;
        default: break;
        }
        return UINT_MAX;
    }
    

    bool OpenGLFrameBuffer::Validate()
    {
        Bind();
        const GLenum Result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        Unbind();
        return Result == GL_FRAMEBUFFER_COMPLETE;
    }
}
