#pragma once
#include "Rendering/Queue.h"
#include <cstdint>

typedef struct VkQueue_T* VkQueue;

namespace Nova::Vulkan
{
    class Queue final : public Nova::Queue
    {
    public:
        void Submit(Nova::CommandBuffer* commandBuffer, Nova::Semaphore* waitSemaphore, Nova::Semaphore* signalSemaphore, Nova::Fence* fence = nullptr, uint32_t waitStagesMask = 0) const override;
        void Submit(const Array<Nova::CommandBuffer*>& commandBuffers, const Array<Nova::Semaphore*>& waitSemaphores, const Array<Nova::Semaphore*>& signalSemaphores, Nova::Fence* fence, uint32_t waitStagesMask) const override;
        bool Present(const Nova::Swapchain& swapchain, const Nova::Semaphore* waitSemaphore, uint32_t imageIndex) const override;

        VkQueue GetHandle() const;
        VkQueue* GetHandlePtr();
        const VkQueue* GetHandlePtr() const;

        void SetIndex(uint32_t index);
        uint32_t GetIndex() const;
        const uint32_t* GetIndexPtr() const;

        bool SameIndex(const Queue& other) const;
        bool SameHandle(const Queue& other) const;
        bool Same(const Queue& other) const;
        bool operator==(const Queue& other) const;
    private:
        VkQueue m_Handle = nullptr;
        uint32_t m_Index = 0xFF;
    };
}
