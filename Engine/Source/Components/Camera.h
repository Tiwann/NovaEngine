#pragma once
#include "../Runtime/Component.h"
#include "Containers/Lazy.h"
#include "Math/Matrix4.h"
#include <cstdint>

namespace Nova
{
    enum class CameraProjectionMode
    {
        Orthographic,
        Perspective
    };

    class Camera : public Component
    {
    public:
        Camera(Entity* owner);

        void OnInit() override;
        void OnUpdate(float deltaTime) override;
        //void OnInspectorGUI(const ImGuiIO& IO) override;
        
        const Matrix4& GetViewMatrix();
        const Matrix4& GetProjectionMatrix();
        const Matrix4& GetViewProjectionMatrix();

        void SetDimensions(uint32_t width, uint32_t height);
        void SetProjectionMode(CameraProjectionMode mode);
        void SetFieldOfView(float fov);
        void SetClipPlanes(float near, float far);
        void SetOrthographicSize(float orthoSize);
        void SetClearColor(const Color& color);

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        float GetNearClipPlane() const;
        float GetFarClipPlane() const;
        CameraProjectionMode GetProjectionMode() const;
        float GetOrthographicSize() const;
        Color GetClearColor() const;
        float GetFieldOfView() const;

    private:
        uint32_t m_Width = 0, m_Height = 0;
        CameraProjectionMode m_ProjectionMode = CameraProjectionMode::Perspective;
        float m_FieldOfView = 45.0f;
        float m_Near = 0.1f;
        float m_Far = 1000.0f;
        float m_OrthoSize = 1.0f;
        Color m_ClearColor = Color::Black;

        Lazy<Matrix4> m_ViewMatrix;
        Lazy<Matrix4> m_ProjectionMatrix;
        Lazy<Matrix4> m_ViewProjectionMatrix;
    };
}

