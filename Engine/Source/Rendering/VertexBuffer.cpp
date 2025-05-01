#include "VertexBuffer.h"
#include "Runtime/Assertion.h"

namespace Nova
{
    VertexBuffer::VertexBuffer(Renderer* Renderer) : Object("Vertex Buffer"), m_Renderer(Renderer), m_Data(nullptr, 0)
    {
    }

    VertexBuffer::VertexBuffer(Renderer* Renderer, const Vertex* Data, size_t Count) : Object("Vertex Buffer"), m_Renderer(Renderer), m_Data(Data, Count)
    {
        NOVA_ASSERT(Data || Count != 0, "Tried to create a Vertex Buffer with no data!");
    }

    size_t VertexBuffer::GetCount() const
    {
        return m_Data.Count();
    }

    void VertexBuffer::SendData(const Vertex* Data, size_t Count)
    {
        NOVA_ASSERT(Data || Count != 0, "Invalid data during uploading");
        m_Data = BufferView(Data, Count);
    }

    void VertexBuffer::SendData(const Array<Vertex>& Vertices)
    {
        SendData(Vertices.Data(), Vertices.Count());
    }
}
