#include "OpenGLIndexBuffer.h"
#include <glad/gl.h>

namespace Nova
{
    OpenGLIndexBuffer::OpenGLIndexBuffer(Renderer* Renderer) : IndexBuffer(Renderer)
    {
    }

    bool OpenGLIndexBuffer::Initialize(const IndexBufferCreateInfo& CreateInfo)
    {
        if (m_Handle != U32_MAX)
        {
            glCreateBuffers(1, &m_Handle);
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)CreateInfo.Size, CreateInfo.Data, GL_STATIC_DRAW);
        return true;
    }

    void OpenGLIndexBuffer::Destroy()
    {
        glDeleteBuffers(1, &m_Handle);
    }

    void OpenGLIndexBuffer::SetDebugName(const String& Name)
    {
        glObjectLabel(GL_BUFFER, m_Handle, Name.Count(), Name.Data());
    }
}
