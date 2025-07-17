#pragma once
#include "Rendering/CommandPool.h"

typedef struct VkCommandPool_T* VkCommandPool;

namespace Nova::Vulkan
{
    class Device;
    class CommandBuffer;


    class CommandPool : public Rendering::CommandPool
    {
    public:
        bool Initialize(const Rendering::CommandPoolCreateInfo& createInfo) override;
        void Destroy() override;
        void Reset() override;

        VkCommandPool GetHandle();
        const VkCommandPool* GetHandle() const;
    private:
        Device* m_Device = nullptr;
        VkCommandPool m_Handle = nullptr;
    };
}


