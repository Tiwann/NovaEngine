#pragma once
#include "Rendering/Semaphore.h"

typedef struct VkSemaphore_T* VkSemaphore;

namespace Nova::Vulkan
{
    class RenderDevice;

    class Semaphore final : public Nova::Semaphore
    {
    public:
        bool Initialize(const SemaphoreCreateInfo& createInfo) override;
        void Destroy() override;

        VkSemaphore GetHandle() const;
        const VkSemaphore* GetHandlePtr() const;
    private:
        VkSemaphore m_Handle = nullptr;
        RenderDevice* m_Device = nullptr;
    };
}
