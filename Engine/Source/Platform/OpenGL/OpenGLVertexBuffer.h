#pragma once
#include "Rendering/VertexBuffer.h"


namespace Nova
{
    struct Vertex;
    
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        explicit OpenGLVertexBuffer(Renderer* Renderer);
        bool Initialize(const VertexBufferCreateInfo& CreateInfo) override;
        void Destroy() override;

    private:
        u32 m_Handle = U32_MAX;
    };
}
