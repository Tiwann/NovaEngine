#include "OpenGLUniformBuffer.h"
#include <glad/gl.h>

namespace Nova
{
    OpenGLUniformBuffer::OpenGLUniformBuffer(Renderer* Owner) : UniformBuffer(Owner)
    {
        glCreateBuffers(1, &m_Handle);
        glBindBuffer(GL_UNIFORM_BUFFER, m_Handle);
    }

    bool OpenGLUniformBuffer::Allocate(size_t Size)
    {
        return false;
    }

    void OpenGLUniformBuffer::Free()
    {
        //glDeleteBuffers()
    }

    void OpenGLUniformBuffer::Copy(const void* Dest, size_t DestSize, size_t Offset)
    {
        glBufferData(GL_UNIFORM_BUFFER, DestSize, Dest, GL_DYNAMIC_DRAW);
    }
}
