#include "CommandPool.h"
#include "CommandBuffer.h"
#include "Renderer.h"
#include "Platform/Vulkan/VulkanCommandBuffer.h"

namespace Nova
{
    CommandBuffer* CommandPool::AllocateCommandBuffer(const CommandBufferAllocateInfo& AllocateInfo)
    {
        CommandBuffer* Cmd = nullptr;

        switch (m_Owner->GetGraphicsApi())
        {
        case GraphicsApi::None: return nullptr;
        case GraphicsApi::OpenGL: return nullptr;
        case GraphicsApi::Vulkan: Cmd = new VulkanCommandBuffer(this); break;
        case GraphicsApi::D3D12: return nullptr;
        default: return nullptr;
        }

        if (!Cmd->Initialize(AllocateInfo))
        {
            delete Cmd;
            return nullptr;
        }
        return Cmd;
    }

    void CommandPool::FreeCommandBuffer(CommandBuffer* CommandBuffer)
    {
        CommandBuffer->Destroy();
        delete CommandBuffer;
    }
}
