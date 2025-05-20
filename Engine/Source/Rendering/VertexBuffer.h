#pragma once
#include "RendererObject.h"
#include "Vertex.h"

namespace Nova
{
    class Renderer;

    struct VertexBufferCreateInfo
    {
        const Vertex* Data = nullptr;
        const size_t Count = 0;
    };

    class VertexBuffer : public RendererObject<VertexBufferCreateInfo>
    {
    public:
        explicit VertexBuffer(Renderer* Owner);
        size_t GetCount() const;
        size_t GetSize() const;
    protected:
        size_t m_Count = 0;
        size_t m_Size = 0;
    };
}
