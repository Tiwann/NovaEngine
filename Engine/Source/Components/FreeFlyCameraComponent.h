﻿#pragma once
#include "Runtime/Component.h"

namespace Nova
{
    class Camera;


    class FreeFlyCameraComponent : public Component
    {
    public:
        explicit FreeFlyCameraComponent(Entity* Owner) : Component(Owner, "Free Fly Camera Component")
        {
        }

        void OnInit() override;
        void OnUpdate(f32 Delta) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;
        void SetCamera(Camera* Camera);
    private:
        Camera* m_Camera = nullptr;
        f32 m_Sensitivity = 0.1f;
        f32 m_Speed = 2.0f;
        f32 m_Smoothing = 1.0f;
        f32 m_Yaw = 0.0f;
        f32 m_Pitch = 0.0f;
    };
}
