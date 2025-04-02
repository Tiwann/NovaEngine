#include "OpenGLVertexArray.h"
#include "Core/Graphics/VertexBufferLayout.h"
#include <glad/gl.h>



namespace Hydro
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

    void OpenGLVertexArray::SetBufferLayout(const VertexBufferLayout& Layout)
    {
        for (size_t Att = 0; Att < Layout.Count(); Att++)
        {
            const VertexAttribute& Attribute = Layout[Att];
            const size_t Count = GetNumComponents(Attribute.Type);
            glEnableVertexAttribArray((GLuint)Att);
            glVertexAttribPointer((GLuint)Att, (GLint)Count, GL_FLOAT, GL_FALSE, (GLsizei)Layout.Stride(), (const void*)Layout.GetOffset(Attribute));
        }
    }
}
