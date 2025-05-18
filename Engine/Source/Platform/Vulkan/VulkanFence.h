#pragma once
#include "Rendering/Fence.h"

typedef struct VkFence_T* VkFence;

namespace Nova
{
    class Renderer;

    class VulkanFence : public Fence
    {
    public:
        explicit VulkanFence(Renderer* Owner);

        bool Initialize(const FenceCreateInfo& CreateInfo) override;
        void Reset() override;
        void SetDebugName(const String& Name) override;
        void Destroy() override;

        VkFence GetHandle() const;
        const VkFence* GetHandlePtr() const;

    private:
        VkFence m_Handle = nullptr;
    };

}
