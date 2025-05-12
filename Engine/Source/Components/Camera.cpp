#include "Camera.h"
#include "Math/LinearAlgebra.h"
#include "Transform.h"
#include "Runtime/Application.h"
#include "Runtime/Entity.h"
#include "Runtime/Window.h"
#include "Editor/EditorGUI.h"

namespace Nova
{
    Camera::Camera(Entity* Owner) : Component(Owner, "Camera")
    {                             
    }


    void Camera::OnInit()
    {
        Component::OnInit();
        Transform* Transform = GetTransform();
        Transform->OnChanged.Bind([this]
        {
            m_ViewMatrix.SetDirty();
        });
    }

    void Camera::OnUpdate(f32 Delta)
    {
        const Window* Window = g_Application->GetWindow();
        const f32 NewWidth = Window->GetWidth<f32>();
        const f32 NewHeight = Window->GetHeight<f32>();
        if (Math::AreDifferent(m_Settings.Width, NewWidth) || Math::AreDifferent(m_Settings.Height, NewHeight))
        {
            m_Settings.Width = NewWidth;
            m_Settings.Height = NewHeight;
            m_ProjectionMatrix.SetDirty();
        }
    }

    void Camera::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);

        if (UI::DragValue<f32>("Width", m_Settings.Width, 1, 0, 0, "%.0f"))
        {
            m_ProjectionMatrix.SetDirty();
        }

        if (UI::DragValue<f32>("Height", m_Settings.Height, 1, 0, 0, "%.0f"))
        {
            m_ProjectionMatrix.SetDirty();
        }

        if (UI::DragValue<f32>("Near Plane", m_Settings.NearPlane))
        {
            m_ProjectionMatrix.SetDirty();
        }
        if (UI::DragValue<f32>("Far Plane", m_Settings.FarPlane))
        {
            m_ProjectionMatrix.SetDirty();
        }
        ImGui::ColorPicker4("Clear Color", (f32*)&ClearColor);

        const char* ProjectionTypes[2] = { "Perspective", "Orthographic" };
        if (ImGui::Combo("Projection", (int*)&m_Settings.ProjectionMode, ProjectionTypes, 2))
        {
            m_ProjectionMatrix.SetDirty();
        }

        if (m_Settings.ProjectionMode == CameraProjectionMode::Orthographic)
        {
            if (UI::DragValue<f32>("Orthographic Size", m_Settings.OrthoSize))
            {
                m_ProjectionMatrix.SetDirty();
            }
        }
        else
        {
            if (UI::DragValue<f32>("Field Of View", m_Settings.FieldOfView, 1.0f, 0.0f, 180.0f))
            {
                m_ProjectionMatrix.SetDirty();
            }
        }
    }

    
    const Matrix4& Camera::GetViewMatrix()
    {
        const auto ComputeView = [&]() -> Matrix4
        {
            const Transform* Transform = GetTransform();
            Matrix4 View = Matrix4::Identity;
            View.Translate(-Transform->GetPosition());
            View.RotateDegrees(-Transform->GetRotation());
            return View;
        };

        return m_ViewMatrix.Get(ComputeView);
    }

    const Matrix4& Camera::GetProjectionMatrix()
    {
        const auto ComputeProjection = [&]() -> Matrix4
        {
            const f32 AspectRatio = m_Settings.Width / m_Settings.Height;
            const Matrix4 Projection = m_Settings.ProjectionMode == CameraProjectionMode::Perspective ?
                Math::Perspective(
                m_Settings.FieldOfView,
                AspectRatio,
                m_Settings.NearPlane,
                m_Settings.FarPlane)

            : Math::Orthographic(
                m_Settings.Width,
                m_Settings.Height,
                m_Settings.OrthoSize,
                m_Settings.NearPlane,
                m_Settings.FarPlane);

            return Projection;
        };
        return m_ProjectionMatrix.Get(ComputeProjection);
    }

    void Camera::SetSettings(const CameraSettings& Settings)
    {
        m_Settings = Settings;
        m_ProjectionMatrix.SetDirty();
    }

    const CameraSettings& Camera::GetSettings() const
    {
        return m_Settings;
    }

    void Camera::SetProjectionMode(const CameraProjectionMode Mode)
    {
        m_Settings.ProjectionMode = Mode;
        m_ProjectionMatrix.SetDirty();
    }

    void Camera::SetFieldOfView(const f32 FieldOfView)
    {
        m_Settings.FieldOfView = FieldOfView;
        m_ProjectionMatrix.SetDirty();
    }

    void Camera::SetClipPlanes(const f32 Near, const f32 Far)
    {
        m_Settings.NearPlane = Near;
        m_Settings.FarPlane = Far;
        m_ProjectionMatrix.SetDirty();
    }

    void Camera::SetOrthographicSize(f32 Size)
    {
        m_Settings.OrthoSize = Size;
        m_ProjectionMatrix.SetDirty();
    }
}

