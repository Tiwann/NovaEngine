#include "Buffer.h"
#include "RenderDevice.h"
#include <glad/glad.h>

namespace Nova::OpenGL
{
    Buffer::Buffer(Buffer&& other) noexcept
    {
        if (this == &other)
            return;

        m_Handle = other.m_Handle;
        m_Device = other.m_Device;
        m_Size = other.m_Size;
        m_Usage = other.m_Usage;

        other.m_Handle = 0xFFFFFFFF;
        other.m_Device = nullptr;
        other.m_Size = 0;
        other.m_Usage = BufferUsage::None;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept
    {
        if (this == &other)
            return *this;

        m_Handle = other.m_Handle;
        m_Device = other.m_Device;
        m_Size = other.m_Size;
        m_Usage = other.m_Usage;

        other.m_Handle = 0xFFFFFFFF;
        other.m_Device = nullptr;
        other.m_Size = 0;
        other.m_Usage = BufferUsage::None;
        return *this;
    }

    bool Buffer::Initialize(const BufferCreateInfo& createInfo)
    {
        if (m_Handle != 0xFFFFFFFF) 
            glDeleteBuffers(1, &m_Handle);
        
        glCreateBuffers(1, &m_Handle);
        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        m_Device = device;
        m_Size = createInfo.size;
        m_Usage = createInfo.usage;
        return true;
    }

    void Buffer::Destroy()
    {
        glDeleteBuffers(1, &m_Handle);
    }

    bool Buffer::Resize(const size_t newSize, bool keepData)
    {
        BufferCreateInfo bufferCreateInfo;
        bufferCreateInfo.size = newSize;
        bufferCreateInfo.device = m_Device;
        bufferCreateInfo.usage = m_Usage;

        Buffer newBuffer;
        newBuffer.Initialize(bufferCreateInfo);

        if (!GPUCopy(newBuffer, 0, 0, newSize))
            return false;

        Destroy();

        *this = std::move(newBuffer);
        return false;
    }

    bool Buffer::CPUCopy(const void* src, const size_t offset, const size_t size)
    {
        if (m_Handle == 0xFFFFFFFF) return false;
        if (!src) return false;
        if (!size) return false;

        GLenum usage = GL_INVALID_INDEX;
        switch (m_Usage)
        {
        case BufferUsage::None: usage = GL_INVALID_INDEX; break;
        case BufferUsage::VertexBuffer: usage = GL_ARRAY_BUFFER; break;
        case BufferUsage::IndexBuffer: usage = GL_ELEMENT_ARRAY_BUFFER; break;
        case BufferUsage::UniformBuffer: usage = GL_UNIFORM_BUFFER; break;
        case BufferUsage::StorageBuffer: usage = GL_SHADER_STORAGE_BUFFER; break;
        case BufferUsage::StagingBuffer: break;
        }

        glNamedBufferData(m_Handle, size, src, usage);
        return true;
    }

    bool Buffer::CPUCopy(const size_t offset, const size_t size, void* outBuffer)
    {
        return true;
    }

    bool Buffer::GPUCopy(Nova::Buffer& other, const size_t srcOffset, const size_t destOffset, const size_t size)
    {
        return true;
    }

    void Buffer::Memset(const size_t value, const size_t size)
    {
        void* mapped = glMapNamedBuffer(m_Handle, GL_WRITE_ONLY);
        Memory::Memset(mapped, value, size);
        glUnmapNamedBuffer(m_Handle);
    }
}
