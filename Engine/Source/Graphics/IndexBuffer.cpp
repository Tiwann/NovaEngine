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

    IndexBuffer* IndexBuffer::Create(GraphicsApi const& GraphicsApi)
    {
        switch (GraphicsApi) {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return new OpenGLIndexBuffer();
        case GraphicsApi::Vulkan: return new VulkanIndexBuffer();
        case GraphicsApi::D3D12: return new D3D12IndexBuffer();
        default: return nullptr;
        }
    }

    IndexBuffer* IndexBuffer::Create(const u32* Indices, size_t Count, GraphicsApi const& GraphicsApi)
    {
        switch (GraphicsApi) {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return new OpenGLIndexBuffer(Indices, Count);
        case GraphicsApi::Vulkan: return new VulkanIndexBuffer(Indices, Count);
        case GraphicsApi::D3D12: return new D3D12IndexBuffer(Indices, Count);
        default: return nullptr;
        }
    }

    IndexBuffer* IndexBuffer::Create(const Array<u32>& Indices, GraphicsApi const& GraphicsApi)
    {
        return Create(Indices.Data(), Indices.Count(), GraphicsApi);
    }

    size_t IndexBuffer::Count() const
    {
        return m_Data.Count();
    }
}
