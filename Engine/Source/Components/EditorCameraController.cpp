#include "EditorCameraController.h"

#include "Camera.h"
#include "Transform.h"
#include "Editor/ViewportPanel.h"
#include "Input/Input.h"
#include "Editor/EditorGUI.h"
#include "Editor/Selection.h"
#include "Runtime/Application.h"
#include "Runtime/DesktopWindow.h"

namespace Nova
{
    EditorCameraController::EditorCameraController(Entity* Owner) : Component(Owner, "Editor Camera Controller")
    {
    }

    void EditorCameraController::OnInit()
    {
        Component::OnInit();
    }

    void EditorCameraController::OnUpdate(const f32 Delta)
    {
        Component::OnUpdate(Delta);
        if (!m_Camera) return;

        const DesktopWindow* Window = g_Application->GetWindow()->As<DesktopWindow>();

        const Vector2 MouseDelta = Window->GetDeltaMousePosition();
        Transform* CameraTransform = m_Camera->GetTransform();
        EntityHandle SelectedEntity = Selection::GetEntity();
        m_Pivot = SelectedEntity ? SelectedEntity->GetTransform()->GetPosition() : Vector3::Zero;


        bool TryingPan = Window->GetCombined(KeyCode::LeftShift, MouseButton::Middle);
        bool TryingZoom = Window->GetCombined(KeyCode::LeftControl, MouseButton::Middle);
        bool TryingRotate = Window->GetMouseButton(MouseButton::Middle);

        if (TryingPan)
        {
            TryingZoom = false;
            TryingRotate = false;
        }

        if (TryingZoom)
        {
            TryingPan = false;
            TryingRotate = false;
        }

        if (TryingRotate)
        {
            TryingPan = false;
            TryingZoom = false;
        }

 
        if (TryingPan)
        {
            if (!Math::IsZero(MouseDelta.Magnitude()))
            {
                CameraTransform->Translate(m_PanSpeed * MouseDelta.x * Delta, m_PanSpeed * -MouseDelta.y * Delta, 0.0f);
            }
        }

        if (TryingZoom)
        {
            if (!Math::IsZero(MouseDelta.y))
            {
                CameraTransform->Translate(0.0f, 0.0, m_ZoomSpeed * MouseDelta.y * Delta);
            }
        }

        if (TryingRotate)
        {
            if (!Math::IsZero(MouseDelta.Magnitude()))
            {
                CameraTransform->RotateAround({m_RotationSpeed * MouseDelta.y * Delta, m_RotationSpeed * MouseDelta.x * Delta, 0.0}, m_Pivot);
            }
        }
    }

    void EditorCameraController::OnDestroy()
    {
        Component::OnDestroy();
    }

    void EditorCameraController::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
        UI::DragValue("Pan Speed", m_PanSpeed, 0.01f, 0.0f);
        UI::DragValue("Rotation Speed", m_RotationSpeed, 0.01f, 0.0f);
        UI::DragValue("Zoom Speed", m_ZoomSpeed, 0.01f, 0.0f);
    }

    Camera* EditorCameraController::GetCamera() const
    {
        return m_Camera;
    }

    void EditorCameraController::SetCamera(Camera* Camera)
    {
        m_Camera = Camera;
    }
}
