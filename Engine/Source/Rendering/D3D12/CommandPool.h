#pragma once
#include "CommandBuffer.h"
#include "Rendering/CommandPool.h"

struct ID3D12CommandAllocator;

namespace Nova::D3D12
{
    class RenderDevice;
    class Queue;

    class CommandPool final : public Nova::CommandPool
    {
    public:
        bool Initialize(const CommandPoolCreateInfo& createInfo) override;
        void Destroy() override;
        void Reset() override;

        ID3D12CommandAllocator* GetHandle() { return m_Handle; }
        const ID3D12CommandAllocator* GetHandle() const { return m_Handle; }

        Queue* GetQueue() { return m_Queue; }
        const Queue* GetQueue() const { return m_Queue; }

        RenderDevice* GetDevice() { return m_Device; }
        const RenderDevice* GetDevice() const { return m_Device; }
        CommandBuffer AllocateCommandBuffer(CommandBufferLevel level);

    private:
        RenderDevice* m_Device = nullptr;
        Queue* m_Queue = nullptr;
        ID3D12CommandAllocator* m_Handle = nullptr;
    };
}
