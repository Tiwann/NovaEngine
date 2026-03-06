#include "FreeFlyCameraComponent.h"
#include "Components/Transform.h"
#include "Math/Vector2.h"
#include "Runtime/DesktopWindow.h"
#include "Components/Camera.h"
#include "Runtime/Application.h"

namespace Nova
{
    void FreeFlyCameraComponent::OnInit()
    {
        Component::OnInit();
    }

    void FreeFlyCameraComponent::OnUpdate(const float deltaTime)
    {
        Transform* camTransform = m_Camera->GetTransform();
        Application& application = Application::GetCurrentApplication();
        Ref<DesktopWindow> window = application.GetWindow().As<DesktopWindow>();
        if (!window) return;

        const Vector2 mouseDelta = window->GetDeltaMousePosition();
        Vector3 moveDirection = Vector3::Zero;
        float speed = m_Speed;

        if (window->GetMouseButton(MouseButton::Right))
        {
            m_Pitch += mouseDelta.y * m_Sensitivity;
            m_Yaw += mouseDelta.x * m_Sensitivity;
            m_Pitch = Math::Clamp(m_Pitch, -89.0f, 89.0f);

            const Quaternion pitchRotation = Quaternion::FromAxisAngleDegrees(Vector3::Right, m_Pitch);
            const Quaternion yawRotation = Quaternion::FromAxisAngleDegrees(Vector3::Up, m_Yaw);
            const Quaternion finalRotation = pitchRotation * yawRotation;
            camTransform->SetRotation(finalRotation);

            const Vector3 forward = camTransform->GetForwardVector();
            const Vector3 right = camTransform->GetRightVector();

            if (window->GetKey(KeyCode::KeyW)) moveDirection -= forward;
            if (window->GetKey(KeyCode::KeyS)) moveDirection += forward;
            if (window->GetKey(KeyCode::KeyD)) moveDirection += right;
            if (window->GetKey(KeyCode::KeyA)) moveDirection -= right;
            if (window->GetKey(KeyCode::Space)) moveDirection += Vector3::Up;
            if (window->GetKey(KeyCode::LeftControl)) moveDirection += Vector3::Down;

            if (window->GetKey(KeyCode::LeftShift))
                speed *= 4.0f;

            if (!Math::IsZero(moveDirection.Magnitude()))
                moveDirection = Math::Normalize(moveDirection);
        }

        const Vector3 currentPosition = camTransform->GetPosition();
        const Vector3 translation = moveDirection * (speed * deltaTime);
        const Vector3 targetPosition = currentPosition + translation;
        camTransform->SetPosition(targetPosition);

        if (window->GetMouseButtonDown(MouseButton::Right))
        {
            window->SetCursorMode(CursorMode::Locked);
        }

        if (window->GetMouseButtonUp(MouseButton::Right))
        {
            window->SetCursorMode(CursorMode::Hidden);
        }
    }

    void FreeFlyCameraComponent::SetCamera(Camera* Camera)
    {
        m_Camera = Camera;
    }
}
