#include "OpenGLVertexArray.h"
#include "Rendering/VertexLayout.h"
#include <glad/gl.h>

namespace Nova
{
    OpenGLVertexArray::OpenGLVertexArray()
    {
        glCreateVertexArrays(1, &m_Handle);
        glBindVertexArray(m_Handle);
    }

    OpenGLVertexArray::~OpenGLVertexArray()
    {
        glDeleteVertexArrays(1, &m_Handle);
    }

    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_Handle);
    }

    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void OpenGLVertexArray::SetBufferLayout(const VertexLayout& Layout)
    {
        glBindVertexArray(m_Handle);
        for (size_t i = 0; i < Layout.Count(); i++)
        {
            const VertexAttribute& Attribute = Layout[i];
            const size_t Count = GetFormatComponentCount(Attribute.Format);
            glEnableVertexAttribArray((GLuint)i);
            glVertexAttribPointer((GLuint)i, (GLint)Count, GL_FLOAT, GL_FALSE, (GLsizei)Layout.Stride(), (const void*)Layout.GetOffset(Attribute));
        }
    }
}
