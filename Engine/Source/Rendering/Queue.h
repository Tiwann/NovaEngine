#pragma once
#include "Containers/Array.h"
#include "QueueType.h"

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

        QueueType GetQueueType() const { return m_QueueType; }
        void SetQueueType(const QueueType queueType) { m_QueueType = queueType; }

        virtual void Submit(CommandBuffer* commandBuffer, Semaphore* waitSemaphore, Semaphore* signalSemaphore, Fence* fence = nullptr, uint32_t waitStagesMask = 0) const = 0;
        virtual void Submit(const Array<CommandBuffer*>& commandBuffers, const Array<Semaphore*>& waitSemaphores, const Array<Semaphore*>& signalSemaphores, Fence* fence = nullptr, uint32_t waitStagesMask = 0) const = 0;
        virtual bool Present(const Swapchain& swapchain, const Nova::Semaphore* waitSemaphore, uint32_t imageIndex) const = 0;
    protected:
        QueueType m_QueueType = QueueType::None;
    };
}
