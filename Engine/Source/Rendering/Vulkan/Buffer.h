#pragma once
#include "Rendering/Buffer.h"

typedef struct VkBuffer_T* VkBuffer;
typedef struct VmaAllocation_T* VmaAllocation;
struct VmaAllocationInfo2;


namespace Nova::Vulkan
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
        bool GPUCopy(Nova::Buffer& other, size_t srcOffset, size_t destOffset, size_t size) override;

        VkBuffer GetHandle() const;
        const VkBuffer* GetHandlePtr() const;
        void GetAllocationInfo(VmaAllocationInfo2& outAllocationInfo) const;
    private:
        Device* m_Device = nullptr;
        VkBuffer m_Handle = nullptr;
        VmaAllocation m_Allocation = nullptr;
    };
}
