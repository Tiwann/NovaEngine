#pragma once
#include <cstdint>

namespace Nova
{
    class Camera;
    struct Sprite;
    struct Vector3;
}

namespace Nova::Rendering
{
    class Texture;
    class RenderTarget;
    class Device;

    class Renderer2D
    {
    public:
        Renderer2D() = default;

        bool Initialize(Device& device, RenderTarget& renderTarget);
        void Destroy();

        bool BeginScene(const Camera* camera);
        void EndScene();
        void Render();

        void DrawSprite(const Vector3& position, const Sprite& sprite);
    private:
        Device* m_Device = nullptr;
        const Camera* m_Camera = nullptr;
    };
}
