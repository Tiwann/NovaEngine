#include "FreeFlyCameraComponent.h"
#include "Camera.h"
#include "Transform.h"
#include "Editor/EditorGUI.h"
#include "Input/Input.h"
#include "Math/Vector2.h"


namespace Nova
{
    void FreeFlyCameraComponent::OnInit()
    {
        Component::OnInit();
    }

    void FreeFlyCameraComponent::OnUpdate(const f32 Delta)
    {
        if (Input::GetMouseButton(MouseButton::Right))
        {
            Transform* CamTransform = m_Camera->GetTransform();
            const Vector2 MouseDelta = Input::GetMouseDelta();

            Vector3 Rotation = CamTransform->GetRotation();
            Rotation.x += MouseDelta.y * m_Sensitivity;
            Rotation.y += MouseDelta.x * m_Sensitivity;
            Rotation.x = Math::Clamp(Rotation.x, -89.0f, 89.0f);
            CamTransform->SetRotation(Rotation);

            const Vector3 Forward = CamTransform->GetForwardVector();
            const Vector3 Right = CamTransform->GetRightVector();

            Vector3 MoveDirection = Vector3::Zero;
            if (Input::GetKey(KeyCode::KeyW)) MoveDirection -= Forward;
            if (Input::GetKey(KeyCode::KeyS)) MoveDirection += Forward;
            if (Input::GetKey(KeyCode::KeyD)) MoveDirection += Right;
            if (Input::GetKey(KeyCode::KeyA)) MoveDirection -= Right;
            // if (Input::GetKeyDown(KeyCode::LeftShift)) MoveDirection += Vector3::Up;
            // if (Input::GetKeyDown(KeyCode::LeftControl)) MoveDirection -= Vector3::Up;

            if (!Math::IsZero(MoveDirection.Magnitude()))
            {
                MoveDirection = Math::Normalize(MoveDirection);

                f32 Speed = m_Speed;
                if (Input::GetKey(KeyCode::LeftShift))
                    Speed *= 4.0f;

                CamTransform->Translate(MoveDirection * (Speed * Delta));
            }
        }
    }

    void FreeFlyCameraComponent::OnInspectorGUI(const ImGuiIO& IO)
    {
        UI::DragValue("Sensitivity", m_Sensitivity, 0.01f, 0.0f);
        UI::DragValue("Speed", m_Speed, 0.01f, 0.0f);
    }

    void FreeFlyCameraComponent::SetCamera(Camera* Camera)
    {
        m_Camera = Camera;
    }
}
