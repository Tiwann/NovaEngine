#pragma once
#include "Rendering/Buffer.h"

typedef struct VkBuffer_T* VkBuffer;
typedef struct VmaAllocation_T* VmaAllocation;


namespace Nova::Vulkan
{
    class Device;

    class Buffer : public Rendering::Buffer
    {
    public:
        Buffer() : Rendering::Buffer() {}
        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        bool Initialize(const Rendering::BufferCreateInfo& createInfo) override;
        void Destroy() override;
        bool Resize(size_t newSize, bool keepData) override;
        bool CPUCopy(const void* src, size_t offset, size_t size) override;
        bool GPUCopy(Rendering::Buffer& other, size_t srcOffset, size_t destOffset, size_t size) override;


        VkBuffer GetHandle() const;
        const VkBuffer* GetHandlePtr() const;
    private:
        Device* m_Device = nullptr;
        VkBuffer m_Handle = nullptr;
        VmaAllocation m_Allocation = nullptr;
    };
}
