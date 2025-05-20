#pragma once
#include "Rendering/CommandBuffer.h"
#include <vulkan/vulkan.h>

namespace Nova
{
    class VulkanCommandBuffer : public CommandBuffer
    {
    public:
        explicit VulkanCommandBuffer(CommandPool* Owner);
        bool Initialize(const CommandBufferAllocateInfo& AllocateInfo) override;
        void Destroy() override;
        bool IsValid() const override;
        bool Begin(const CommandBufferBeginInfo& BeginInfo) override;
        bool End() override;

        VkCommandBuffer GetHandle() const;
        const VkCommandBuffer* GetHandlePtr() const;
    private:
        VkCommandBuffer m_Handle = nullptr;
    };
}
