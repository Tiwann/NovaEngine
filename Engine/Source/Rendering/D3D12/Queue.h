#pragma once
#include "Rendering/Queue.h"
#include "Rendering/QueueType.h"

struct ID3D12CommandQueue;

namespace Nova::D3D12
{
    class RenderDevice;

    class Queue final : public Nova::Queue
    {
    public:
        bool Initialize(RenderDevice* device, QueueType queueType);
        void Destroy();

        void Submit(Nova::CommandBuffer* commandBuffer, Nova::Semaphore* waitSemaphore, Nova::Semaphore* signalSemaphore, Nova::Fence* fence, uint32_t waitStagesMask) const override;
        void Submit(const Array<Nova::CommandBuffer*>& commandBuffers, const Array<Nova::Semaphore*>& waitSemaphores, const Array<Nova::Semaphore*>& signalSemaphores, Nova::Fence* fence, uint32_t waitStagesMask) const override;
        bool Present(const Nova::Swapchain& swapchain, const Nova::Semaphore* waitSemaphore, uint32_t imageIndex) const override;

        ID3D12CommandQueue* GetHandle() { return m_Handle; }
        const ID3D12CommandQueue* GetHandle() const { return m_Handle; }

        QueueType GetQueueType() const { return m_Type; }

    private:
        QueueType m_Type = QueueType::None;
        RenderDevice* m_Device = nullptr;
        ID3D12CommandQueue* m_Handle = nullptr;
    };
}
