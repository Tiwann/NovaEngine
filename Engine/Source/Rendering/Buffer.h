#pragma once
#include "Runtime/Object.h"

namespace Nova
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

        BufferCreateInfo& WithDevice(Device* inDevice) { device = inDevice; return *this; }
        BufferCreateInfo& WithUsage(const BufferUsage inUsage) { usage = inUsage; return *this; }
        BufferCreateInfo& WithSize(const size_t inSize) { size = inSize; return *this; }
    };

    class Buffer : public Object
    {
    public:
        Buffer() : Object("Buffer") {}
        ~Buffer() override = default;

        virtual bool Initialize(const BufferCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;

        virtual bool Resize(size_t newSize, bool keepData = true) = 0;
        virtual bool CPUCopy(const void* src, size_t offset, size_t size) = 0;
        virtual bool CPUCopy(size_t offset, size_t size, void* outBuffer) = 0;
        virtual bool GPUCopy(Buffer& other, size_t srcOffset, size_t destOffset, size_t size) = 0;

        template<typename T>
        bool CPUCopy(BufferView<T> buffer, size_t offset)
        {
            return CPUCopy(buffer.Data(), offset, Math::Min(buffer.Size(), m_Size));
        }

        virtual void Memset(size_t value, size_t size) = 0;
        BufferUsage GetUsage() const { return m_Usage; }
        BufferUsage GetUsage() { return m_Usage; }
        size_t GetSize() const { return m_Size; }
    protected:
        size_t m_Size = 0;
        BufferUsage m_Usage = BufferUsage::None;
    };
}
