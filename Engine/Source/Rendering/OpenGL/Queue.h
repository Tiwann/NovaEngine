#pragma once
#include "Rendering/Queue.h"
#include "Rendering/QueueType.h"

namespace Nova::OpenGL
{
    class CommandBuffer;

    class Queue final : public Nova::Queue
    {
    public:
        explicit Queue(const QueueType type) : Nova::Queue(), m_Type{type} {}

        QueueType GetQueueType() const { return m_Type; }
        void Submit(Nova::CommandBuffer* commandBuffer, Semaphore* waitSemaphore, Semaphore* signalSemaphore, Fence* fence, uint32_t waitStagesMask) const override;
        void Submit(const Array<Nova::CommandBuffer*>& commandBuffers, const Array<Semaphore*>& waitSemaphores, const Array<Semaphore*>& signalSemaphores, Fence* fence, uint32_t waitStagesMask) const override;
        bool Present(const Nova::Swapchain& swapchain, const Nova::Semaphore* waitSemaphore, uint32_t imageIndex) const override;

    private:
        QueueType m_Type = QueueType::None;
    };
}