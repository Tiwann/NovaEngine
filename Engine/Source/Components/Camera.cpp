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
        Settings.Width = Window->GetWidth();
        Settings.Height = Window->GetHeight();
    }

    void Camera::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);

        if (UI::DragValue<f32>("Width", Settings.Width, 1, 0, 0, "%.0f"))
        {
            m_ProjectionMatrix.SetDirty();
        }

        if (UI::DragValue<f32>("Height", Settings.Height, 1, 0, 0, "%.0f"))
        {
            m_ProjectionMatrix.SetDirty();
        }

        if (UI::DragValue<f32>("Near Plane", Settings.NearPlane))
        {
            m_ProjectionMatrix.SetDirty();
        }
        if (UI::DragValue<f32>("Far Plane", Settings.FarPlane))
        {
            m_ProjectionMatrix.SetDirty();
        }
        ImGui::ColorPicker4("Clear Color", (f32*)&ClearColor);

        const char* ProjectionTypes[2] = { "Perspective", "Orthographic" };
        if (ImGui::Combo("Projection", (int*)&Settings.Projection, ProjectionTypes, 2))
        {
            m_ProjectionMatrix.SetDirty();
        }

        if (Settings.Projection == CameraProjectionType::Orthographic)
        {
            if (UI::DragValue<f32>("Orthographic Size", Settings.OrthoSize))
            {
                m_ProjectionMatrix.SetDirty();
            }
        }
        else
        {
            if (UI::DragValue<f32>("Field Of View", Settings.FieldOfView, 1.0f, 0.0f, 180.0f))
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
            View.Rotate(-Transform->GetRotation());
            return View;
        };

        return m_ViewMatrix.Get(ComputeView);
    }

    const Matrix4& Camera::GetProjectionMatrix()
    {
        const auto ComputeProjection = [&]() -> Matrix4
        {
            const f32 AspectRatio = Settings.Width / Settings.Height;
            const Matrix4 Projection = Settings.Projection == CameraProjectionType::Perspective ?
                Math::Perspective(
                Settings.FieldOfView,
                AspectRatio,
                Settings.NearPlane,
                Settings.FarPlane)

            : Math::Orthographic(
                Settings.Width,
                Settings.Height,
                Settings.OrthoSize,
                Settings.NearPlane,
                Settings.FarPlane);

            return Projection;
        };
        return m_ProjectionMatrix.Get(ComputeProjection);
    }
}

