#include "Queue.h"
#include "CommandBuffer.h"
#include "Fence.h"
#include "Semaphore.h"
#include "Swapchain.h"
#include <vulkan/vulkan.h>


namespace Nova::Vulkan
{
    void Queue::Submit(Nova::CommandBuffer* commandBuffer, Nova::Semaphore* waitSemaphore, Nova::Semaphore* signalSemaphore, Nova::Fence* fence, const uint32_t waitStagesMask) const
    {
        const CommandBuffer* vkCmdBuff = (CommandBuffer*)commandBuffer;
        const Semaphore* vkWaitSem = (Semaphore*)waitSemaphore;
        const Semaphore* vkSignalSem = (Semaphore*)signalSemaphore;
        const Fence* vkFence = (Fence*)fence;

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = vkCmdBuff->GetHandlePtr();
        submitInfo.waitSemaphoreCount = vkWaitSem ? 1 : 0;
        submitInfo.pWaitSemaphores = vkWaitSem ? vkWaitSem->GetHandlePtr() : nullptr;
        submitInfo.signalSemaphoreCount = vkSignalSem ? 1 : 0;
        submitInfo.pSignalSemaphores = vkSignalSem ? vkSignalSem->GetHandlePtr() : nullptr;
        submitInfo.pWaitDstStageMask = &waitStagesMask;

        vkQueueSubmit(m_Handle, 1, &submitInfo, vkFence ? vkFence->GetHandle() : nullptr);
    }

    void Queue::Submit(const Array<Nova::CommandBuffer*>& commandBuffers, const Array<Nova::Semaphore*>& waitSemaphores, const Array<Nova::Semaphore*>& signalSemaphores, Nova::Fence* fence, const uint32_t waitStagesMask) const
    {
        Array<VkCommandBuffer> cmdBuffs;
        Array<VkSemaphore> waitSems;
        Array<VkSemaphore> signalSems;

        for (const Nova::CommandBuffer* commandBuffer : commandBuffers)
            cmdBuffs.Add(((const CommandBuffer*)commandBuffer)->GetHandle());

        for (const Nova::Semaphore* waitSemaphore : waitSemaphores)
            waitSems.Add(((const Semaphore*)waitSemaphore)->GetHandle());

        for (const Nova::Semaphore* signalSemaphore : signalSemaphores)
            signalSems.Add(((const Semaphore*)signalSemaphore)->GetHandle());

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = cmdBuffs.Count();
        submitInfo.pCommandBuffers = cmdBuffs.Data();
        submitInfo.waitSemaphoreCount = waitSems.Count();
        submitInfo.pWaitSemaphores = waitSems.Data();
        submitInfo.signalSemaphoreCount = signalSems.Count();
        submitInfo.pSignalSemaphores = signalSems.Data();
        submitInfo.pWaitDstStageMask = &waitStagesMask;

        vkQueueSubmit(m_Handle, 1, &submitInfo, fence ? ((Fence*)fence)->GetHandle() : nullptr);
    }

    bool Queue::Present(const Nova::Swapchain& swapchain, const Nova::Semaphore* waitSemaphore, const uint32_t imageIndex) const
    {
        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = ((Swapchain&)swapchain).GetHandlePtr();
        presentInfo.pImageIndices = &imageIndex;
        if (waitSemaphore)
        {
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = ((Semaphore&)*waitSemaphore).GetHandlePtr();
        }

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

    void Queue::SetIndex(const uint32_t index)
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
