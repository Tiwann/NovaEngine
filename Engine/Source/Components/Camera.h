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

        void SetSettings(const CameraSettings& Settings);
        const CameraSettings& GetSettings() const;
        void SetProjectionMode(CameraProjectionMode Mode);
        void SetFieldOfView(f32 FieldOfView);
        void SetClipPlanes(f32 Near, f32 Far);
        void SetOrthographicSize(f32 Size);
        ClearMode ClearMode = ClearMode::Color;
        Color ClearColor = Color::Black;
    private:
        CameraSettings m_Settings;
        Lazy<Matrix4> m_ViewMatrix;
        Lazy<Matrix4> m_ProjectionMatrix;
    };
}

