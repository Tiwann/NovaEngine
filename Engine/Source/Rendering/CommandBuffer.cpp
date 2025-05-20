#include "CommandBuffer.h"

namespace Nova
{
    CommandBuffer::CommandBuffer(CommandPool* Owner)
        : Object("Command Buffer"), m_CommandPool(Owner)
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
