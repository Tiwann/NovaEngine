#pragma once
#include "Runtime/Component.h"
#include "Math/LinearAlgebra.h"

namespace Nova
{
    class Camera;
    
    class EditorCameraController : public Component
    {
    public:
        EditorCameraController(Entity* Owner);

        void OnInit() override;
        void OnUpdate(f32 Delta) override;
        void OnDestroy() override;
        void OnInspectorGUI(const ImGuiIO& IO) override;

        Camera* GetCamera() const;
        void SetCamera(Camera* Camera);
    private:
        Camera* m_Camera = nullptr;
        f32 m_PanSpeed = 5.0f;
        f32 m_RotationSpeed = 0.0f;
        f32 m_ZoomSpeed = 5.0f;
        Vector3 m_Pivot;
    };
}
