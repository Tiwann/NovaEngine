#include "RenderTarget.h"

namespace Nova
{
    RenderTarget::RenderTarget(Renderer* Owner) : Object("Render Target"), m_Owner(Owner)
    {
    }

    Renderer* RenderTarget::GetOwner() const
    {
        return m_Owner;
    }

    u32 RenderTarget::GetWidth() const
    {
        return m_Width;
    }

    u32 RenderTarget::GetHeight() const
    {
        return m_Height;
    }
}
