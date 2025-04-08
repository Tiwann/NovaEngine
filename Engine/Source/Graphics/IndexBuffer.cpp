#include "IndexBuffer.h"
#include "Platform/PlatformIndexBuffer.h"

namespace Nova
{
    IndexBuffer::IndexBuffer(): m_Data(nullptr, 0)
    {

    }

    IndexBuffer::IndexBuffer(const u32* Indices, size_t Count) : m_Data(Indices, Count)
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

    IndexBuffer* IndexBuffer::Create()
    {
        NOVA_RHI_PLATFORM_RETURN(IndexBuffer);
    }

    IndexBuffer* IndexBuffer::Create(const u32* Indices, size_t Count)
    {
        NOVA_RHI_PLATFORM_RETURN(IndexBuffer, Indices, Count);
    }

    IndexBuffer* IndexBuffer::Create(const Array<u32>& Indices)
    {
        return Create(Indices.Data(), Indices.Count());
    }

    size_t IndexBuffer::Count() const
    {
        return m_Data.Count();
    }
}
