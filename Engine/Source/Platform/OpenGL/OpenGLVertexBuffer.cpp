#include "OpenGLVertexBuffer.h"
#include <glad/gl.h>

namespace Nova
{
    OpenGLVertexBuffer::OpenGLVertexBuffer(Renderer* Renderer) : VertexBuffer(Renderer)
    {
        glCreateBuffers(1, &m_Handle);
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(Renderer* Renderer, const Vertex* Data, size_t Count) : VertexBuffer(Renderer, Data, Count)
    {
        glCreateBuffers(1, &m_Handle);
        glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_Data.Size(), m_Data.Data(), GL_STATIC_DRAW);
    }

    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        glDeleteBuffers(1, &m_Handle);
    }

    void OpenGLVertexBuffer::SendData(const Vertex* Data, size_t Count)
    {
        VertexBuffer::SendData(Data, Count);
        glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_Data.Size(), m_Data.Data(), GL_STATIC_DRAW);
    }

    void OpenGLVertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_Handle);
    }
}
