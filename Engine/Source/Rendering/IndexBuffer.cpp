#include "IndexBuffer.h"

namespace Nova
{
    IndexBuffer::IndexBuffer(Renderer* Renderer) : m_Renderer(Renderer), m_Data(nullptr, 0)
    {

    }

    IndexBuffer::IndexBuffer(Renderer* Renderer, const u32* Indices, size_t Count) : m_Renderer(Renderer),  m_Data(Indices, Count)
    {
    }

    void IndexBuffer::SendData(const u32* Indices, size_t Count)
    {
        m_Data = BufferView(Indices, Count);
    }

    void IndexBuffer::SendData(const Array<u32>& Indices)
    {
        SendData(Indices.Data(), Indices.Count());
    }

    size_t IndexBuffer::Count() const
    {
        return m_Data.Count();
    }
}
