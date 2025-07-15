#pragma once
#include "Containers/Array.h"
#include <cstdint>

typedef struct VkQueue_T* VkQueue;

namespace Nova::Vulkan
{
    class CommandBuffer;
    class Semaphore;
    class Fence;
    class Swapchain;

    class Queue
    {
    public:
        void Submit(CommandBuffer* commandBuffer, Semaphore* waitSemaphore, Semaphore* signalSemaphore, Fence* fence = nullptr, uint32_t waitStagesMask = 0) const;
        void Submit(const Array<CommandBuffer*>& commandBuffers, const Array<Semaphore*>& waitSemaphores, const Array<Semaphore*>& signalSemaphores, Fence* fence = nullptr, uint32_t waitStagesMask = 0) const;
        bool Present(const Swapchain& swapchain, const Semaphore& waitSemaphore, uint32_t imageIndex);

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
