#pragma once
#include "Rendering/IndexBuffer.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Nova
{
    class VulkanIndexBuffer : public IndexBuffer
    {
    public:
        explicit VulkanIndexBuffer(Renderer* Owner);
        bool Initialize(const IndexBufferCreateInfo& CreateInfo) override;
        void SetDebugName(const String& Name) override;
        void Destroy() override;

        VkBuffer GetHandle() const;
    private:
        VkBuffer            m_Handle = nullptr;
        VmaAllocation       m_Allocation = nullptr;
        VmaAllocationInfo   m_AllocationInfo = VmaAllocationInfo{ };
    };
}
