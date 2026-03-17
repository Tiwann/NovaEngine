#include "CommandPool.h"
#include "RenderDevice.h"
#include <directx/d3d12.h>

#include "Conversions.h"


namespace Nova::D3D12
{
    bool CommandPool::Initialize(const CommandPoolCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        if (!createInfo.queue) return false;

        RenderDevice* device = (RenderDevice*)createInfo.device;
        Queue* queue = (Queue*)createInfo.queue;

        const D3D12_COMMAND_LIST_TYPE listType = Convert<D3D12_COMMAND_LIST_TYPE>(queue->GetQueueType());
        ID3D12Device13* deviceHandle = device->GetHandle();
        ID3D12CommandAllocator* commandAllocator = nullptr;
        if (DX_FAILED(deviceHandle->CreateCommandAllocator(listType, IID_PPV_ARGS(&commandAllocator))))
            return false;

        if (m_Handle) m_Handle->Reset();

        if (DX_FAILED(commandAllocator->QueryInterface(IID_PPV_ARGS(&m_Handle))))
            return false;

        m_Device = device;
        m_Queue = queue;
        return true;
    }

    void CommandPool::Destroy()
    {
        m_Handle->Release();
    }

    void CommandPool::Reset()
    {
        m_Handle->Reset();
    }

    QueueType CommandPool::GetQueueType() const
    {
        return m_Queue->GetQueueType();
    }

    CommandBuffer CommandPool::AllocateCommandBuffer(CommandBufferLevel level)
    {
        CommandBufferAllocateInfo allocateInfo;
        allocateInfo.commandPool = this;
        allocateInfo.device = m_Device;
        allocateInfo.level = level;

        CommandBuffer cmdBuff;
        cmdBuff.Allocate(allocateInfo);
        return cmdBuff;
    }
}
