#include "OpenGLVertexBuffer.h"
#include <glad/gl.h>

namespace Nova
{
    OpenGLVertexBuffer::OpenGLVertexBuffer(Renderer* Renderer) : VertexBuffer(Renderer)
    {

    }

    bool OpenGLVertexBuffer::Initialize(const VertexBufferCreateInfo& CreateInfo)
    {
        if (m_Handle != U32_MAX)
        {
            glDeleteBuffers(1, &m_Handle);
        }

        glCreateBuffers(1, &m_Handle);
        glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(CreateInfo.Count * sizeof(Vertex)), CreateInfo.Data, GL_STATIC_DRAW);
        return true;
    }

    void OpenGLVertexBuffer::Destroy()
    {
        glDeleteBuffers(1, &m_Handle);
        m_Handle = U32_MAX;
    }
}
