#pragma once
#include "Rendering/Buffer.h"

typedef struct VkBuffer_T* VkBuffer;
typedef struct VmaAllocation_T* VmaAllocation;
struct VmaAllocationInfo2;


namespace Nova::Vulkan
{
    class RenderDevice;

    class Buffer final : public Nova::Buffer
    {
    public:
        Buffer() = default;

        bool Initialize(const BufferCreateInfo& createInfo) override;
        void Destroy() override;

        void* Map() override;
        void Unmap(const void* ptr) override;

        VkBuffer GetHandle() const;
        const VkBuffer* GetHandlePtr() const;
        void GetAllocationInfo(VmaAllocationInfo2& outAllocationInfo) const;
    private:
        RenderDevice* m_Device = nullptr;
        VkBuffer m_Handle = nullptr;
        VmaAllocation m_Allocation = nullptr;

    };
}
