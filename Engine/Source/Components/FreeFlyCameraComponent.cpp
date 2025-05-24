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
        Transform* CamTransform = m_Camera->GetTransform();
        const Vector2 MouseDelta = Input::GetMouseDelta();
        Vector3 MoveDirection = Vector3::Zero;
        f32 Speed = m_Speed;

        if (Input::GetMouseButton(MouseButton::Right))
        {

            const Quaternion PitchRotation = Quaternion::FromEulerDegrees(MouseDelta.y * m_Sensitivity, 0.0, 0.0f);
            const Quaternion YawRotation = Quaternion::FromEulerDegrees(0.0f, MouseDelta.x * m_Sensitivity, 0.0f);
            CamTransform->Rotate(YawRotation);
            CamTransform->Rotate(PitchRotation);

            const Vector3 Forward = CamTransform->GetForwardVector();
            const Vector3 Right = CamTransform->GetRightVector();

            if (Input::GetKey(KeyCode::KeyW)) MoveDirection -= Forward;
            if (Input::GetKey(KeyCode::KeyS)) MoveDirection += Forward;
            if (Input::GetKey(KeyCode::KeyD)) MoveDirection += Right;
            if (Input::GetKey(KeyCode::KeyA)) MoveDirection -= Right;

            if (Input::GetKey(KeyCode::LeftShift))
                Speed *= 4.0f;

            if (!Math::IsZero(MoveDirection.Magnitude()))
                MoveDirection = Math::Normalize(MoveDirection);
        }

        const Vector3 CurrentPosition = CamTransform->GetPosition();
        const Vector3 Translation = MoveDirection * Speed;
        const Vector3 TargetPosition = CurrentPosition + Translation;
        const Vector3 SmoothedPosition = Vector3::Lerp(CurrentPosition, TargetPosition, m_Smoothing * Delta);
        CamTransform->SetPosition(SmoothedPosition);
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
