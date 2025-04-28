#include "CommandBuffer.h"

namespace Nova
{
    CommandBuffer::CommandBuffer(CommandPool* Owner, const CommandBufferAllocateInfo& AllocateInfo)
        : Object("Command Buffer"), m_CommandPool(Owner), m_Level(AllocateInfo.Level)
    {
    }

    CommandPool* CommandBuffer::GetCommandPool() const
    {
        return m_CommandPool;
    }

    CommandBufferLevel CommandBuffer::GetLevel() const
    {
        return m_Level;
    }
}
