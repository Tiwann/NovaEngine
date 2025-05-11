#pragma once
#include "Runtime/Component.h"
#include "Runtime/CameraSettings.h"
#include "Runtime/LogCategory.h"
#include "Containers/Lazy.h"
#include "Math/Matrix4.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Camera, "CAMERA");

namespace Nova
{
    enum class ClearMode
    {
        Color,
        Shader
    };
    
    class Camera : public Component
    {
    public:
        Camera(Entity* Owner);

        void OnInit() override;
        void OnUpdate(f32 Delta) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;
        
        const Matrix4& GetViewMatrix();
        const Matrix4& GetProjectionMatrix();
        CameraSettings Settings{};
        ClearMode ClearMode = ClearMode::Color;
        Color ClearColor = Color::Black;
    private:
        Lazy<Matrix4> m_ViewMatrix;
        Lazy<Matrix4> m_ProjectionMatrix;
    };
}

