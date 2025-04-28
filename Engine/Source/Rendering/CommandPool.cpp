#include "CommandPool.h"

namespace Nova
{
    CommandPool::CommandPool(Renderer* Owner, const CommandPoolCreateInfo& CreateInfo)
    : Object("Command Pool"), m_Renderer(Owner), m_Flags(CreateInfo.Flags)
    {

    }

    Renderer* CommandPool::GetRenderer() const
    {
        return m_Renderer;
    }
}
