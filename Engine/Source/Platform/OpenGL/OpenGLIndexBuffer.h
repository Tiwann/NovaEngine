#pragma once
#include "Rendering/IndexBuffer.h"

namespace Nova
{
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        explicit OpenGLIndexBuffer(Renderer* Renderer);
        explicit OpenGLIndexBuffer(Renderer* Renderer, const u32* Indices, size_t Count);
        ~OpenGLIndexBuffer() override;

        void SendData(const u32* Indices, size_t Count) override;
        void Bind() const override;
    private:
        u32 m_Handle;
    };
}
