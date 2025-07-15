#include "Queue.h"
#include "CommandBuffer.h"
#include "Fence.h"
#include "Semaphore.h"
#include <vulkan/vulkan.h>

#include "Swapchain.h"

namespace Nova::Vulkan
{
    void Queue::Submit(CommandBuffer* commandBuffer, Semaphore* waitSemaphore, Semaphore* signalSemaphore, Fence* fence, uint32_t waitStagesMask) const
    {
        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffer->GetHandlePtr();
        submitInfo.waitSemaphoreCount = waitSemaphore ? 1 : 0;
        submitInfo.pWaitSemaphores = waitSemaphore ? waitSemaphore->GetHandlePtr() : nullptr;
        submitInfo.signalSemaphoreCount = signalSemaphore ? 1 : 0;
        submitInfo.pSignalSemaphores = signalSemaphore ? signalSemaphore->GetHandlePtr() : nullptr;
        submitInfo.pWaitDstStageMask = &waitStagesMask;

        vkQueueSubmit(m_Handle, 1, &submitInfo, fence ? fence->GetHandle() : nullptr);
    }

    void Queue::Submit(const Array<CommandBuffer*>& commandBuffers, const Array<Semaphore*>& waitSemaphores, const Array<Semaphore*>& signalSemaphores, Fence* fence, uint32_t waitStagesMask) const
    {
        Array<VkCommandBuffer> cmdBuffs;
        Array<VkSemaphore> waitSems;
        Array<VkSemaphore> signalSems;

        for (const CommandBuffer* commandBuffer : commandBuffers)
            cmdBuffs.Add(commandBuffer->GetHandle());

        for (const Semaphore* waitSemaphore : waitSemaphores)
            waitSems.Add(waitSemaphore->GetHandle());

        for (const Semaphore* signalSemaphore : signalSemaphores)
            signalSems.Add(signalSemaphore->GetHandle());

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = cmdBuffs.Count();
        submitInfo.pCommandBuffers = cmdBuffs.Data();
        submitInfo.waitSemaphoreCount = waitSems.Count();
        submitInfo.pWaitSemaphores = waitSems.Data();
        submitInfo.signalSemaphoreCount = signalSems.Count();
        submitInfo.pSignalSemaphores = signalSems.Data();
        submitInfo.pWaitDstStageMask = &waitStagesMask;

        vkQueueSubmit(m_Handle, 1, &submitInfo, fence ? fence->GetHandle() : nullptr);
    }

    bool Queue::Present(const Swapchain& swapchain, const Semaphore& waitSemaphore, const uint32_t imageIndex)
    {
        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchain.GetHandlePtr();
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = waitSemaphore.GetHandlePtr();

        if (vkQueuePresentKHR(m_Handle, &presentInfo) != VK_SUCCESS)
            return false;
        return true;
    }

    VkQueue Queue::GetHandle() const
    {
        return m_Handle;
    }

    VkQueue* Queue::GetHandlePtr()
    {
        return &m_Handle;
    }

    const VkQueue* Queue::GetHandlePtr() const
    {
        return &m_Handle;
    }

    void Queue::SetIndex(uint32_t index)
    {
        m_Index = index;
    }

    uint32_t Queue::GetIndex() const
    {
        return m_Index;
    }

    const uint32_t* Queue::GetIndexPtr() const
    {
        return &m_Index;
    }

    bool Queue::SameIndex(const Queue& other) const
    {
        return m_Index == other.m_Index;
    }

    bool Queue::SameHandle(const Queue& other) const
    {
        return m_Handle == other.m_Handle;
    }

    bool Queue::Same(const Queue& other) const
    {
        return m_Index == other.m_Index && m_Handle == other.m_Handle;
    }

    bool Queue::operator==(const Queue& other) const
    {
        return m_Index == other.m_Index && m_Handle == other.m_Handle;
    }
}
