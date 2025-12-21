#pragma once
#include "Rendering/Buffer.h"

namespace Nova::OpenGL
{
    class Device;

    class Buffer final : public Nova::Buffer
    {
    public:
        Buffer() : Nova::Buffer() {}
        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        bool Initialize(const BufferCreateInfo& createInfo) override;
        void Destroy() override;
        bool Resize(size_t newSize, bool keepData) override;
        bool CPUCopy(const void* src, size_t offset, size_t size) override;
        bool CPUCopy(size_t offset, size_t size, void* outBuffer) override;
        bool GPUCopy(Nova::Buffer& other, size_t srcOffset, size_t destOffset, size_t size) override;

        void Memset(size_t value, size_t size) override;

        uint32_t GetHandle() const { return m_Handle; }
    private:
        Device* m_Device = nullptr;
        uint32_t m_Handle = 0xFFFFFFFF;
    };
}
