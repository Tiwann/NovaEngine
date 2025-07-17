#pragma once
#include <cstdint>

namespace Nova::Rendering
{
    class Device;
    class Texture;
    class RenderTarget;

    class Renderer2D
    {
    public:
        explicit Renderer2D(Device& device);

        void DrawTexture(Texture& srcTexture, uint32_t x, uint32_t y, RenderTarget* destRenderTarget);
    private:
        Device& m_Device;
    };
}
