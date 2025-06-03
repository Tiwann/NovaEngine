#include "OpenGLRenderTarget.h"
#include <glad/gl.h>

#include "OpenGLRenderer.h"

namespace Nova
{
    OpenGLRenderTarget::OpenGLRenderTarget(Renderer* Owner): RenderTarget(Owner)
    {
    }

    bool OpenGLRenderTarget::Initialize(const RenderTargetCreateInfo& Info)
    {
        const OpenGLRenderer* Renderer = m_Owner->As<OpenGLRenderer>();
        const OpenGLRendererTypeConvertor& Convertor = Renderer->Convertor;
        glCreateFramebuffers(1, &m_Handle);

        for (size_t AttachmentIndex = 0; AttachmentIndex < Info.AttachmentInfos.Count(); AttachmentIndex++)
        {
            const RenderTargetAttachmentInfo& AttachmentInfo = Info.AttachmentInfos[AttachmentIndex];
            OpenGLRenderTargetAttachment* Attachment = new OpenGLRenderTargetAttachment();

            glCreateTextures(GL_TEXTURE_2D, 1, &Attachment->Handle);
            glTextureParameteri(Attachment->Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTextureParameteri(Attachment->Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTextureParameteri(Attachment->Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(Attachment->Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureStorage2D(Attachment->Handle, 1, Convertor.ConvertFormat(AttachmentInfo.Format), Info.Width, Info.Height);
            glNamedFramebufferTexture(m_Handle, GL_COLOR_ATTACHMENT0, Attachment->Handle, 0);
            m_Attachments.Add(Attachment);
        }

        return true;
    }

    bool OpenGLRenderTarget::Resize(const u32 Width, const u32 Height)
    {
        if (m_Width == Width && m_Height == Height)
            return true;

        const OpenGLRenderer* Renderer = m_Owner->As<OpenGLRenderer>();
        const OpenGLRendererTypeConvertor& Convertor = Renderer->Convertor;

        for (size_t AttachmentIndex = 0; AttachmentIndex < m_Attachments.Count(); AttachmentIndex++)
        {
            const OpenGLRenderTargetAttachment* Attachment = (OpenGLRenderTargetAttachment*)m_Attachments[AttachmentIndex];
            glTextureStorage2D(Attachment->Handle, 1, Convertor.ConvertFormat(Attachment->Format), Width, Height);
        }
        m_Width = Width;
        m_Height = Height;
        return true;
    }

    void OpenGLRenderTarget::Destroy()
    {
        for (size_t AttachmentIndex = 0; AttachmentIndex < m_Attachments.Count(); AttachmentIndex++)
        {
            const OpenGLRenderTargetAttachment* Attachment = (OpenGLRenderTargetAttachment*)m_Attachments[AttachmentIndex];
            glDeleteTextures(1, &Attachment->Handle);
            delete Attachment;
        }
        m_Attachments.Clear();
        glDeleteFramebuffers(1, &m_Handle);
    }
}
