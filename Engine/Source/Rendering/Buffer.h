#pragma once

namespace Nova::Rendering
{
    class Device;

    enum class BufferUsage
    {
        None,
        VertexBuffer,
        IndexBuffer,
        UniformBuffer,
        StorageBuffer,
        StagingBuffer
    };

    struct BufferCreateInfo
    {
        Device* device;
        BufferUsage usage = BufferUsage::None;
        size_t size = 0;
    };

    class Buffer
    {
    public:
        Buffer() = default;
        virtual ~Buffer() = default;

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        virtual bool Initialize(const BufferCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual bool Resize(size_t newSize, bool keepData = true) = 0;
        virtual bool CopyData(const void* src, size_t offset, size_t size) = 0;
        virtual bool CopyTo(const Buffer& other, size_t srcOffset, size_t destOffset, size_t size) = 0;
    protected:
        size_t m_Size = 0;
        BufferUsage m_Usage = BufferUsage::None;
    };
}
