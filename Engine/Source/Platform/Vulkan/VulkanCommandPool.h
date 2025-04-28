#pragma once
#include "Rendering/CommandPool.h"
#include <vulkan/vulkan.h>

namespace Nova
{
    class VulkanCommandPool : public CommandPool
    {
    public:
        explicit VulkanCommandPool(Renderer* Owner, const CommandPoolCreateInfo& CreateInfo);
        ~VulkanCommandPool() override;

        VkCommandPool GetHandle() const;
        CommandBuffer* AllocateCommandBuffer(const CommandBufferAllocateInfo& AllocateInfo) override;
        void FreeCommandBuffer(CommandBuffer* CommandBuffer) override;

    private:
        VkCommandPool m_Handle = nullptr;
    };
}
