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

    VertexBuffer* VertexBuffer::Create(GraphicsApi const& GraphicsApi)
    {
        switch (GraphicsApi) {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return new OpenGLVertexBuffer();
        case GraphicsApi::Vulkan: return new VulkanVertexBuffer();
        case GraphicsApi::D3D12: return new D3D12VertexBuffer();
        default: return nullptr;
        }
    }

    VertexBuffer* VertexBuffer::Create(const Vertex* Data, size_t Count, GraphicsApi const& GraphicsApi)
    {
        switch (GraphicsApi) {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return new OpenGLVertexBuffer(Data, Count);
        case GraphicsApi::Vulkan: return new VulkanVertexBuffer(Data, Count);
        case GraphicsApi::D3D12: return new D3D12VertexBuffer(Data, Count);
        default: return nullptr;
        }
    }

    VertexBuffer* VertexBuffer::Create(const Array<Vertex>& Vertices, GraphicsApi const& GraphicsApi)
    {
        return Create(Vertices.Data(), Vertices.Count(), GraphicsApi);
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
