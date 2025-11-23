#pragma once
#include "Rendering/Fence.h"

typedef struct VkFence_T* VkFence;

namespace Nova::Vulkan
{
    class Device;

    class Fence final : public Nova::Fence
    {
    public:
        bool Initialize(const FenceCreateInfo& createInfo) override;
        void Destroy() override;
        void Wait(uint64_t timeoutNs) override;
        void Reset() override;

        VkFence GetHandle() const;
        const VkFence* GetHandlePtr() const;
    private:
        Device* m_Device = nullptr;
        VkFence m_Handle = nullptr;
    };
}
