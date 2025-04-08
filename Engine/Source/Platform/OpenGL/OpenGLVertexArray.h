#pragma once
#include "Graphics/VertexArray.h"
#include "Runtime/Types.h"


namespace Nova
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
