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

        other.m_Handle = INVALID_HANDLE<uint32_t>;
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

        other.m_Handle = INVALID_HANDLE<uint32_t>;
        other.m_Device = nullptr;
        other.m_Size = 0;
        other.m_Usage = BufferUsage::None;
        return *this;
    }

    bool Buffer::Initialize(const BufferCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (createInfo.size <= 0) return false;
        if (createInfo.usage == BufferUsage::None) return false;

        if (HandleIsValid(m_Handle))
            glDeleteBuffers(1, &m_Handle);
        
        glCreateBuffers(1, &m_Handle);
        glNamedBufferStorage(m_Handle, createInfo.size, nullptr, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
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

        if (!CopyDataTo(newBuffer, 0, 0, newSize))
            return false;

        Destroy();

        *this = std::move(newBuffer);
        return false;
    }

    bool Buffer::WriteData(const void* src, const size_t offset, const size_t size)
    {
        if (m_Handle == 0xFFFFFFFF) return false;
        if (!src) return false;
        if (!size) return false;

        void* mapped = glMapNamedBufferRange(m_Handle, 0, size, GL_MAP_WRITE_BIT);
        Memory::Memcpy(mapped, src, size);
        glUnmapNamedBuffer(m_Handle);
        return true;
    }

    bool Buffer::CopyDataTo(const size_t offset, const size_t size, void* outBuffer)
    {
        if (!outBuffer || !size || !HandleIsValid(m_Handle))
            return false;

        const void* mapped = glMapNamedBufferRange(m_Handle, offset, size, GL_MAP_READ_BIT);
        Memory::Memcpy(outBuffer, mapped, size);
        glUnmapNamedBuffer(m_Handle);
        return true;
    }

    bool Buffer::CopyDataTo(Nova::Buffer& other, const size_t srcOffset, const size_t destOffset, const size_t size)
    {
        const Buffer& otherBuffer = static_cast<Buffer&>(other);
        if (!HandleIsValid(m_Handle) || !HandleIsValid(otherBuffer.m_Handle))
            return false;

        glCopyNamedBufferSubData(m_Handle, otherBuffer.m_Handle, srcOffset, destOffset, size);
        return true;
    }

    void Buffer::Memset(const size_t value, const size_t size)
    {
        void* mapped = glMapNamedBufferRange(m_Handle, 0, size, GL_MAP_WRITE_BIT);
        Memory::Memset(mapped, value, size);
        glUnmapNamedBuffer(m_Handle);
    }
}
