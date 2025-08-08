#pragma once
#include "SpriteRendererFlags.h"

namespace Nova
{
    namespace Rendering
    {
        class Device;
        class CommandBuffer;
        class RenderPass;
    }

    class Camera;
    struct Matrix4;
    struct Sprite;
    struct Color;
    struct Vector2;

    struct Renderer2DCreateInfo
    {
        Rendering::Device* device = nullptr;
        Rendering::RenderPass* renderPass = nullptr;
        size_t maxQuads = 512;
    };

    class Renderer2D
    {
    public:
        Renderer2D() = default;

        bool Initialize(const Renderer2DCreateInfo& createInfo);

        bool BeginScene(Camera* camera);
        void EndScene();
        void Render(Rendering::CommandBuffer& cmdBuffer);

        void DrawSprite(const Matrix4& transform, const Sprite& sprite, const Vector2& tiling, const Color& colorTint, const SpriteRendererFlags& flags);
    private:
        Rendering::Device* m_Device = nullptr;
        Camera* m_Camera = nullptr;
    };
}
