#pragma once
#include "Rendering/CommandBuffer.h"
#include "Rendering/CommandPool.h"

typedef struct VkCommandPool_T* VkCommandPool;

namespace Nova::Vulkan
{
    class Device;
    class CommandBuffer;


    class CommandPool final : public Nova::CommandPool
    {
    public:
        bool Initialize(const CommandPoolCreateInfo& createInfo) override;
        void Destroy() override;
        void Reset() override;

        VkCommandPool GetHandle();
        const VkCommandPool* GetHandle() const;

        CommandBuffer AllocateCommandBuffer(CommandBufferLevel level);
    private:
        Device* m_Device = nullptr;
        VkCommandPool m_Handle = nullptr;
    };
}


