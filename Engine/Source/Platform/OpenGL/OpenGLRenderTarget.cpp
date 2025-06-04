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
        return true;
    }

    bool OpenGLRenderTarget::Resize(const u32 Width, const u32 Height)
    {
        if (m_Width == Width && m_Height == Height)
            return true;

        const OpenGLRenderer* Renderer = m_Owner->As<OpenGLRenderer>();
        const OpenGLRendererTypeConvertor& Convertor = Renderer->Convertor;
        m_Width = Width;
        m_Height = Height;
        return true;
    }

    void OpenGLRenderTarget::Destroy()
    {

    }
}
