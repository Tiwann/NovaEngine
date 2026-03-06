#pragma once
#include "Runtime/Component.h"

namespace Nova
{
    class Camera;


    class FreeFlyCameraComponent final : public Component
    {
    public:
        explicit FreeFlyCameraComponent(Entity* Owner) : Component(Owner, "Free Fly Camera Component")
        {
        }

        void OnInit() override;
        void OnUpdate(float deltaTime) override;
        void SetCamera(Camera* Camera);

    private:
        Camera* m_Camera = nullptr;
        float m_Sensitivity = 0.1f;
        float m_Speed = 2.0f;
        float m_Smoothing = 1.0f;
        float m_Yaw = 0.0f;
        float m_Pitch = 0.0f;
    };
}
