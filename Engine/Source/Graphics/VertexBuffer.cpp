#include "VertexBuffer.h"

#include "Runtime/Assertion.h"
#include "Platform/PlatformVertexBuffer.h"

namespace Nova
{
    VertexBuffer::VertexBuffer() : m_Data(nullptr, 0)
    {
    }

    VertexBuffer::VertexBuffer(const Vertex* Data, size_t Count) : m_Data(Data, Count)
    {
        NOVA_ASSERT(Data || Count != 0, "Tried to create a Vertex Buffer with no data!");
    }

    VertexBuffer* VertexBuffer::Create()
    {
        NOVA_RHI_PLATFORM_RETURN(VertexBuffer);
    }

    VertexBuffer* VertexBuffer::Create(const Vertex* Data, size_t Count)
    {
        NOVA_RHI_PLATFORM_RETURN(VertexBuffer, Data, Count);
    }

    VertexBuffer* VertexBuffer::Create(const Array<Vertex>& Vertices)
    {
        return Create(Vertices.Data(), Vertices.Count());
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
