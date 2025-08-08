#include "Camera.h"
#include "Math/LinearAlgebra.h"
#include "Transform.h"
#include "Game/Entity.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"

namespace Nova
{
    Camera::Camera(Entity* owner) : Component(owner, "Camera")
    {                             
    }


    void Camera::OnInit()
    {
        Component::OnInit();
        Transform* transform = GetTransform();
        transform->onChanged.Bind([&]
        {
            m_ViewMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        });
    }

    void Camera::OnUpdate(const float deltaTime)
    {
        Component::OnUpdate(deltaTime);
        const Application* application = GetApplication();
        const Window* window = application->GetWindow();
        const float newWidth = window->GetWidth();
        const float newHeight = window->GetHeight();

        if (Math::AreDifferent(m_Width, newWidth) || Math::AreDifferent(m_Height, newHeight))
        {
            m_Width = newWidth;
            m_Height = newHeight;
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }
    }

    /*void Camera::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);

        if (UI::DragValue<f32>("Width", m_Settings.Width, 1, 0, 0, "%.0f"))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        if (UI::DragValue<f32>("Height", m_Settings.Height, 1, 0, 0, "%.0f"))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        if (UI::DragValue<f32>("Near Plane", m_Settings.NearPlane))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        if (UI::DragValue<f32>("Far Plane", m_Settings.FarPlane))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        ImGui::ColorEdit4("Clear Color", (f32*)&ClearColor);

        const char* ProjectionTypes[2] = { "Perspective", "Orthographic" };
        if (ImGui::Combo("Projection", (int*)&m_Settings.ProjectionMode, ProjectionTypes, 2))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        if (m_Settings.ProjectionMode == CameraProjectionMode::Orthographic)
        {
            if (UI::DragValue<f32>("Orthographic Size", m_Settings.OrthoSize))
            {
                m_ProjectionMatrix.SetDirty();
                m_ViewProjectionMatrix.SetDirty();
            }
        }
        else
        {
            if (UI::DragValue<f32>("Field Of View", m_Settings.FieldOfView, 1.0f, 0.0f, 180.0f))
            {
                m_ProjectionMatrix.SetDirty();
                m_ViewProjectionMatrix.SetDirty();
            }
        }
    }*/


    const Matrix4& Camera::GetViewMatrix()
    {
        const auto computeView = [&]() -> Matrix4
        {
            const Transform* transform = GetTransform();
            Matrix4 view = Matrix4::Identity;
            view.Translate(-transform->GetPosition());
            view.RotateDegrees(-transform->GetRotation().ToEulerDegrees());
            return view;
        };

        return m_ViewMatrix.Get(computeView);
    }

    const Matrix4& Camera::GetProjectionMatrix()
    {
        const Function<Matrix4()> computeProjection = [&]() -> Matrix4
        {
            const float aspectRatio = (float)m_Width / m_Height;
            const Matrix4 projection = m_ProjectionMode == CameraProjectionMode::Perspective ?
                Math::Perspective(
                m_FieldOfView,
                aspectRatio,
                m_Near,
                m_Far)

            : Math::Orthographic(
                m_Width,
                m_Height,
                m_OrthoSize,
                m_Near,
                m_Far);

            return projection;
        };

        return m_ProjectionMatrix.Get(computeProjection);
    }

    const Matrix4& Camera::GetViewProjectionMatrix()
    {
        const auto computeViewProjection = [&]() -> Matrix4
        {
            return GetProjectionMatrix() * GetViewMatrix();
        };
        return m_ViewProjectionMatrix.Get(computeViewProjection);
    }

    void Camera::SetDimensions(const uint32_t width, const uint32_t height)
    {
        m_Width = width;
        m_Height = height;
        m_ProjectionMatrix.SetDirty();
        m_ViewProjectionMatrix.SetDirty();
    }

    void Camera::SetProjectionMode(const CameraProjectionMode mode)
    {
        m_ProjectionMode = mode;
        m_ProjectionMatrix.SetDirty();
        m_ViewProjectionMatrix.SetDirty();
    }

    void Camera::SetFieldOfView(const float fov)
    {
        m_FieldOfView = fov;
        m_ProjectionMatrix.SetDirty();
        m_ViewProjectionMatrix.SetDirty();
    }

    void Camera::SetClipPlanes(const float near, const float far)
    {
        m_Near = near;
        m_Far = far;
        m_ProjectionMatrix.SetDirty();
        m_ViewProjectionMatrix.SetDirty();
    }

    void Camera::SetOrthographicSize(const float orthoSize)
    {
        m_OrthoSize = orthoSize;
        m_ProjectionMatrix.SetDirty();
        m_ViewProjectionMatrix.SetDirty();
    }

    void Camera::SetClearColor(const Color& color)
    {
        m_ClearColor = color;
    }

    uint32_t Camera::GetWidth() const
    {
        return m_Width;
    }

    uint32_t Camera::GetHeight() const
    {
        return m_Height;
    }

    float Camera::GetNearClipPlane() const
    {
        return m_Near;
    }

    float Camera::GetFarClipPlane() const
    {
        return m_Far;
    }

    CameraProjectionMode Camera::GetProjectionMode() const
    {
        return m_ProjectionMode;
    }

    float Camera::GetOrthographicSize() const
    {
        return m_OrthoSize;
    }

    Color Camera::GetClearColor() const
    {
        return m_ClearColor;
    }

    float Camera::GetFieldOfView() const
    {
        return m_FieldOfView;
    }
}

