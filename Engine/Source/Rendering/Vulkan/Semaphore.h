﻿#pragma once
#include "Rendering/Semaphore.h"

typedef struct VkSemaphore_T* VkSemaphore;

namespace Nova::Vulkan
{
    class Device;

    class Semaphore : public Rendering::Semaphore
    {
    public:
        bool Initialize(const Rendering::SemaphoreCreateInfo& createInfo) override;
        void Destroy() override;

        VkSemaphore GetHandle() const;
        const VkSemaphore* GetHandlePtr() const;
    private:
        VkSemaphore m_Handle = nullptr;
        Device* m_Device = nullptr;
    };
}
