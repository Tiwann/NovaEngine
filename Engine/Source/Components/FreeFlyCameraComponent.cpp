#include "FreeFlyCameraComponent.h"
#include "Camera.h"
#include "Transform.h"
#include "Editor/EditorGUI.h"
#include "Input/Input.h"
#include "Math/Vector2.h"
#include <GLFW/glfw3.h>

#include "Runtime/Application.h"
#include "Runtime/DesktopWindow.h"
#include "Runtime/Window.h"

namespace Nova
{
    void FreeFlyCameraComponent::OnInit()
    {
        Component::OnInit();
    }

    void FreeFlyCameraComponent::OnUpdate(const f32 Delta)
    {
        Transform* CamTransform = m_Camera->GetTransform();
        const Vector2 MouseDelta = Input::GetMouseDelta();
        Vector3 MoveDirection = Vector3::Zero;
        f32 Speed = m_Speed;

        if (Input::GetMouseButton(MouseButton::Right))
        {
            m_Pitch += MouseDelta.y * m_Sensitivity;
            m_Yaw += MouseDelta.x * m_Sensitivity;
            m_Pitch = Math::Clamp(m_Pitch, -89.0f, 89.0f);

            const Quaternion PitchRotation = Quaternion::FromAxisAngleDegrees(Vector3::Right, m_Pitch);
            const Quaternion YawRotation = Quaternion::FromAxisAngleDegrees(Vector3::Up, m_Yaw);
            const Quaternion FinalRotation = YawRotation * PitchRotation;
            CamTransform->SetRotation(FinalRotation);

            const Vector3 Forward = CamTransform->GetForwardVector();
            const Vector3 Right = CamTransform->GetRightVector();

            if (Input::GetKey(KeyCode::KeyW)) MoveDirection -= Forward;
            if (Input::GetKey(KeyCode::KeyS)) MoveDirection += Forward;
            if (Input::GetKey(KeyCode::KeyD)) MoveDirection += Right;
            if (Input::GetKey(KeyCode::KeyA)) MoveDirection -= Right;
            if (Input::GetKey(KeyCode::Space)) MoveDirection += Vector3::Up;
            if (Input::GetKey(KeyCode::LeftControl)) MoveDirection += Vector3::Down;

            if (Input::GetKey(KeyCode::LeftShift))
                Speed *= 4.0f;

            if (!Math::IsZero(MoveDirection.Magnitude()))
                MoveDirection = Math::Normalize(MoveDirection);
        }

        const Vector3 CurrentPosition = CamTransform->GetPosition();
        const Vector3 Translation = MoveDirection * (Speed * Delta);
        const Vector3 TargetPosition = CurrentPosition + Translation;
        CamTransform->SetPosition(TargetPosition);

        DesktopWindow* Window = g_Application->GetWindow()->As<DesktopWindow>();
        if (!Window) return;

        if (Window->GetMouseButtonDown(MouseButton::Right))
        {
            glfwSetInputMode(Window->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        if (Window->GetMouseButtonUp(MouseButton::Right))
        {
            glfwSetInputMode(Window->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    void FreeFlyCameraComponent::OnInspectorGUI(const ImGuiIO& IO)
    {
        UI::DragValue("Sensitivity", m_Sensitivity, 0.01f, 0.0f);
        UI::DragValue("Speed", m_Speed, 0.01f, 0.0f);
        UI::DragValue("Smoothing", m_Smoothing, 0.1, 0.1f, 10.0f);
    }

    void FreeFlyCameraComponent::SetCamera(Camera* Camera)
    {
        m_Camera = Camera;
    }
}
