#pragma once
#include "Rendering/Queue.h"
#include <cstdint>

typedef struct VkQueue_T* VkQueue;

namespace Nova::Vulkan
{
    class CommandBuffer;
    class Semaphore;
    class Fence;
    class Swapchain;

    class Queue : public Rendering::Queue
    {
    public:
        void Submit(Rendering::CommandBuffer* commandBuffer, Rendering::Semaphore* waitSemaphore, Rendering::Semaphore* signalSemaphore, Rendering::Fence* fence = nullptr, uint32_t waitStagesMask = 0) const override;
        void Submit(const Array<Rendering::CommandBuffer*>& commandBuffers, const Array<Rendering::Semaphore*>& waitSemaphores, const Array<Rendering::Semaphore*>& signalSemaphores, Rendering::Fence* fence, uint32_t waitStagesMask) const override;
        bool Present(const Rendering::Swapchain& swapchain, const Rendering::Semaphore& waitSemaphore, uint32_t imageIndex) const override;

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
