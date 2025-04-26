#pragma once
#include "Rendering/UniformBuffer.h"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Nova
{
    class VulkanUniformBuffer : public UniformBuffer
    {
    public:
        explicit VulkanUniformBuffer(Renderer* Owner);

        bool Allocate(size_t Size) override;
        void Free() override;
        void Copy(const void* Dest, size_t DestSize, size_t Offset) override;
        VkBuffer GetHandle() const { return m_Handle; }
    private:
        VmaAllocation m_Allocation = nullptr;
        VmaAllocationInfo m_AllocationInfo;
        VkBuffer m_Handle = nullptr;
    };
}
