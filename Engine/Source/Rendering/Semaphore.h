#pragma once

namespace Nova
{
    class RenderDevice;

    struct SemaphoreCreateInfo
    {
        RenderDevice* device = nullptr;
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
