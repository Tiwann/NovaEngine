#pragma once
#include "Rendering/VertexBuffer.h"


namespace Nova
{
    struct Vertex;
    
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        explicit OpenGLVertexBuffer(Renderer* Renderer);
        explicit OpenGLVertexBuffer(Renderer* Renderer, const Vertex* Data, size_t Count);
        ~OpenGLVertexBuffer() override;
        
        void SendData(const Vertex* Data, size_t Count) override;
        void Bind() const override;
    private:
        u32 m_Handle;
    };
}
