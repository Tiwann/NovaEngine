#include "Renderer2D.h"
#include "Device.h"

namespace Nova::Rendering
{
    Renderer2D::Renderer2D(Device& device): m_Device(device)
    {

    }

    void Renderer2D::DrawTexture(Texture& srcTexture, uint32_t x, uint32_t y, RenderTarget* destRenderTarget)
    {
        if (destRenderTarget)
        {
            m_Device.BlitToRenderTarget(srcTexture, *destRenderTarget, x, y);
        } else
        {
            //m_Device.BlitToSwapchain()
        }
    }
}
