#pragma once
#include "Rendering/IndexBuffer.h"

namespace Nova
{
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer();
        ~OpenGLIndexBuffer() override;
        OpenGLIndexBuffer(const u32* Indices, size_t Count);

        void SendData(const u32* Indices, size_t Count) override;
        void Bind() const override;
    private:
        u32 m_Handle;
    };
}
