#pragma once
#include "Rendering/IndexBuffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Nova
{
    class VulkanIndexBuffer : public IndexBuffer
    {
    public:
        explicit VulkanIndexBuffer(Renderer* Renderer);
        explicit VulkanIndexBuffer(Renderer* Renderer, const u32* Indices, size_t Count);
        ~VulkanIndexBuffer() override;

        void Bind() const override;
        void SendData(const u32* Indices, size_t Count) override;
        VkBuffer GetHandle() const;
    private:
        VkBuffer            m_Handle = nullptr;
        VmaAllocation       m_Allocation = nullptr;
        VmaAllocationInfo   m_AllocationInfo = VmaAllocationInfo{ };
    };
}
