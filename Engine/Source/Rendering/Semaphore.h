#pragma once

namespace Nova
{
    class Device;

    struct SemaphoreCreateInfo
    {
        Device* device = nullptr;
    };

    class Semaphore
    {
    public:
        Semaphore() = default;
        virtual ~Semaphore() = default;

        virtual bool Initialize(const SemaphoreCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
    };
}
