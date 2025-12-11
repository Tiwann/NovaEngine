#include "Queue.h"
#include "Device.h"

#include <directx/d3d12.h>
#include <dxgi1_6.h>


namespace Nova::D3D12
{

    bool Queue::Initialize(Device* device, QueueType queueType)
    {
        if (!device) return false;
        auto* deviceHandle = device->GetHandle();
        if (!deviceHandle) return false;

        if (m_Handle) m_Handle->Release();
        switch (queueType)
        {
        case QueueType::None: return false;
        case QueueType::Graphics:
            {
                D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
                commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
                commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
                commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                commandQueueDesc.NodeMask = 0;
                if (DX_FAILED(deviceHandle->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_Handle))))
                    return false;
                break;
            }

        case QueueType::Compute:
            {
                D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
                commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
                commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
                commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                commandQueueDesc.NodeMask = 0;
                if (DX_FAILED(deviceHandle->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_Handle))))
                    return false;
                break;
            }

        case QueueType::Transfer:
            {
                D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
                commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
                commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
                commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
                commandQueueDesc.NodeMask = 0;
                if (DX_FAILED(deviceHandle->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_Handle))))
                    return false;
                break;
            }

        default: return false;
        }

        m_Device = device;
        m_Type = queueType;
        return true;
    }

    void Queue::Destroy()
    {
        if (m_Handle) m_Handle->Release();
    }

    void Queue::Submit(Nova::CommandBuffer* commandBuffer, Nova::Semaphore* waitSemaphore, Nova::Semaphore* signalSemaphore, Nova::Fence* fence, uint32_t waitStagesMask) const
    {
        CommandBuffer* cmdBuffer = static_cast<CommandBuffer*>(commandBuffer);
        const ID3D12GraphicsCommandList10* cmdBufferHandle = cmdBuffer->GetHandle();
        m_Handle->ExecuteCommandLists(1, (ID3D12CommandList**)&cmdBufferHandle);

        if (fence)
        {
            Fence* dxFence = static_cast<Fence*>(fence);
            ID3D12Fence1* fenceHandle = dxFence->GetHandle();
            const uint64_t fenceValue = dxFence->GetValue();

            (void)m_Handle->Signal(fenceHandle, fenceValue);
        }
    }

    void Queue::Submit(const Array<Nova::CommandBuffer*>& commandBuffers, const Array<Nova::Semaphore*>& waitSemaphores, const Array<Nova::Semaphore*>& signalSemaphores, Nova::Fence* fence, uint32_t waitStagesMask) const
    {
    }

    bool Queue::Present(const Nova::Swapchain& swapchain, const Nova::Semaphore* waitSemaphore, uint32_t imageIndex) const
    {
        const Swapchain& swap = (const Swapchain&)swapchain;
        IDXGISwapChain4* swapchainHandle = const_cast<IDXGISwapChain4*>(swap.GetHandle());
        if (!swapchainHandle) return false;
        if (DX_FAILED(swapchainHandle->Present(m_Device->HasVSync(), 0)))
            return false;
        return true;
    }
}
