#pragma once
#include "Rendering/Semaphore.h"

typedef struct VkSemaphore_T* VkSemaphore;

namespace Nova
{
    class VulkanSemaphore : public Semaphore
    {
    public:
        explicit VulkanSemaphore(Renderer* Owner);

        bool Initialize(const SemaphoreCreateInfo& CreateInfo) override;
        void Destroy() override;
        void SetDebugName(const String& Name) override;

        VkSemaphore GetHandle() const;
        const VkSemaphore* GetHandlePtr() const;

    private:
        VkSemaphore m_Handle = nullptr;
    };
}
