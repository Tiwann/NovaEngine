#pragma once
#include "Graphics/IndexBuffer.h"
#include "Platform/PlatformRenderer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Nova
{
    class VulkanIndexBuffer : public IndexBuffer
    {
    public:
        VulkanIndexBuffer();
        VulkanIndexBuffer(const u32* Indices, size_t Count);
        ~VulkanIndexBuffer() override;

        void Bind() const override;
        void SendData(const u32* Indices, size_t Count) override;

        void Invalidate() const;
    private:
        VulkanRenderer*     m_Renderer = nullptr;
        VkBuffer            m_Handle = nullptr;
        VmaAllocation       m_Allocation = nullptr;
        VmaAllocationInfo   m_AllocationInfo = VmaAllocationInfo{ };
    };
}
