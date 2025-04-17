#include "OpenGLIndexBuffer.h"
#include <glad/gl.h>

namespace Nova
{
    OpenGLIndexBuffer::OpenGLIndexBuffer(Renderer* Renderer) : IndexBuffer(Renderer)
    {
        glCreateBuffers(1, &m_Handle);
    }

    OpenGLIndexBuffer::OpenGLIndexBuffer(Renderer* Renderer, const u32* Indices, size_t Count) : IndexBuffer(Renderer, Indices, Count)
    {
        glCreateBuffers(1, &m_Handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)m_Data.Size(), m_Data.Data(), GL_STATIC_DRAW);
    }

    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        glDeleteBuffers(1, &m_Handle);
    }

    void OpenGLIndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
    }

    void OpenGLIndexBuffer::SendData(const u32* Indices, size_t Count)
    {
        IndexBuffer::SendData(Indices, Count);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)m_Data.Size(), m_Data.Data(), GL_STATIC_DRAW);
    }
}
