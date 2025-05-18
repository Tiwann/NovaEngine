#pragma once
#include "Rendering/CommandPool.h"
#include <vulkan/vulkan.h>

namespace Nova
{
    class VulkanCommandPool : public CommandPool
    {
    public:
        explicit VulkanCommandPool(Renderer* Owner);
        bool Initialize(const CommandPoolCreateInfo& CreateInfo) override;
        void Destroy() override;

        void SetDebugName(const String& Name) override;
        CommandBuffer* AllocateCommandBuffer(const CommandBufferAllocateInfo& AllocateInfo) override;
        void FreeCommandBuffer(CommandBuffer* CommandBuffer) override;
        VkCommandPool GetHandle() const;

    private:
        VkCommandPool m_Handle = nullptr;
    };
}
