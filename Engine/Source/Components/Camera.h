#pragma once
#include "Runtime/Component.h"
#include "Runtime/CameraSettings.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Camera, "CAMERA");

namespace Nova
{
    class Matrix4;

    enum class ClearMode
    {
        Color,
        Shader
    };
    
    class Camera : public Component
    {
    public:
        Camera(Entity* Owner);

        void OnUpdate(f32 Delta) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;
        
        Matrix4 GetViewMatrix() const;
        Matrix4 GetProjectionMatrix() const;
        CameraSettings Settings{};
        ClearMode ClearMode = ClearMode::Color;
        Color ClearColor = Color::Black;
    };
}

