#include "Buffer.h"
#include "RenderDevice.h"
#include <glad/glad.h>

namespace Nova::OpenGL
{
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

    void* Buffer::Map()
    {
        void* mapped = glMapNamedBuffer(m_Handle, GL_READ_WRITE);
        return mapped;
    }

    void Buffer::Unmap(const void* ptr)
    {
        (void)ptr;
        glUnmapNamedBuffer(m_Handle);
    }
}
