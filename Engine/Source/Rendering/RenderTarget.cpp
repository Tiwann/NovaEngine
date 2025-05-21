#include "RenderTarget.h"

namespace Nova
{
    RenderTarget::RenderTarget(Renderer* Owner) : RendererObject("Render Target", Owner)
    {
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
