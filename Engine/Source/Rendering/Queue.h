#pragma once
#include "Containers/Array.h"

namespace Nova
{
    class CommandBuffer;
    class Semaphore;
    class Fence;
    class Swapchain;

    class Queue
    {
    public:
        Queue() = default;
        virtual ~Queue() = default;

        virtual void Submit(CommandBuffer* commandBuffer, Semaphore* waitSemaphore, Semaphore* signalSemaphore, Fence* fence = nullptr, uint32_t waitStagesMask = 0) const = 0;
        virtual void Submit(const Array<CommandBuffer*>& commandBuffers, const Array<Semaphore*>& waitSemaphores, const Array<Semaphore*>& signalSemaphores, Fence* fence = nullptr, uint32_t waitStagesMask = 0) const = 0;
        virtual bool Present(const Swapchain& swapchain, const Semaphore& waitSemaphore, uint32_t imageIndex) const = 0;
    };
}
