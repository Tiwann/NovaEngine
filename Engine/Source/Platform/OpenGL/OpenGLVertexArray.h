#pragma once
#include "Core/Graphics/VertexArray.h"
#include "Core/NumericTypes.h"


namespace Hydro
{
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        ~OpenGLVertexArray() override;
        void Bind() const override;
        void Unbind() const override;
        void SetBufferLayout(const VertexBufferLayout& Layout) override;
    private:
        u32 m_Handle;
    };
}
