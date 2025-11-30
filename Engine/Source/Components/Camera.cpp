#include "Camera.h"
#include "Math/LinearAlgebra.h"
#include "Transform.h"
#include "Runtime/Entity.h"
#include "Runtime/Application.h"
#include "Runtime/Window.h"
#include <imgui.h>

namespace Nova
{
    Camera::Camera(Entity* owner) : Component(owner, "Camera")
    {                             
    }


    void Camera::OnInit()
    {
        Component::OnInit();
        Transform* transform = GetTransform();
        transform->OnChanged.Bind([&]
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

    void Camera::OnGui()
    {
        Component::OnGui();

        if (ImGui::DragInt("Width", (int*)&m_Width, 1, 0, 0, "%d"))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        if (ImGui::DragInt("Height", (int*)&m_Height, 1, 0, 0, "%d"))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        if (ImGui::DragFloat("Near Plane", &m_Near, 1, 0, 0))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        if (ImGui::DragFloat("Far Plane", &m_Far, 1, 0, 0))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        ImGui::ColorEdit4("Clear Color", (float*)&m_ClearColor);

        const char* projectionTypes[2] = { "Orthographic", "Perspective" };
        if (ImGui::Combo("Projection", (int*)&m_ProjectionMode, projectionTypes, 2))
        {
            m_ProjectionMatrix.SetDirty();
            m_ViewProjectionMatrix.SetDirty();
        }

        if (m_ProjectionMode == CameraProjectionMode::Orthographic)
        {
            if (ImGui::DragFloat("Orthographic Size", &m_OrthoSize))
            {
                m_ProjectionMatrix.SetDirty();
                m_ViewProjectionMatrix.SetDirty();
            }
        }
        else
        {
            if (ImGui::DragFloat("Field Of View", &m_FieldOfView, 1.0f, 0.0f, 180.0f))
            {
                m_ProjectionMatrix.SetDirty();
                m_ViewProjectionMatrix.SetDirty();
            }
        }
    }


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

