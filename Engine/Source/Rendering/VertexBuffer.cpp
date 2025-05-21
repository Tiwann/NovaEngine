#include "VertexBuffer.h"

namespace Nova
{
    VertexBuffer::VertexBuffer(Renderer* Owner) : RendererObject("Vertex Buffer", Owner)
    {
    }

    size_t VertexBuffer::GetCount() const
    {
        return m_Count;
    }

    size_t VertexBuffer::GetSize() const
    {
        return m_Count;
    }
}
