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

    void Camera::OnUpdate(f32 Delta)
    {
        const Window* Window = g_Application->GetWindow();
        Settings.Width = Window->GetWidth();
        Settings.Height = Window->GetHeight();
    }

    void Camera::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
        
        UI::DragValue<f32>("Width", Settings.Width, 1, 0, 0, "%.0f");
        UI::DragValue<f32>("Height", Settings.Height, 1, 0, 0, "%.0f");
        UI::DragValue<f32>("Near Plane", Settings.NearPlane);
        UI::DragValue<f32>("Far Plane", Settings.FarPlane);
        ImGui::ColorPicker4("Clear Color", (f32*)&ClearColor);

        const char* ProjectionTypes[2] = { "Perspective", "Orthographic" };
        ImGui::Combo("Projection", (int*)&Settings.Projection, ProjectionTypes, 2);
            
        if(Settings.Projection == CameraProjectionType::Orthographic)
        {
            UI::DragValue<f32>("Orthographic Size", Settings.OrthoSize);
        } else
        {
            UI::DragValue<f32>("Field Of View", Settings.FieldOfView);
        }
    }

    
    Matrix4 Camera::GetViewMatrix() const
    {
        const Transform* Transform = GetTransform();
        return Transform->GetWorldSpaceMatrix().Inverse();
    }

    Matrix4 Camera::GetProjectionMatrix() const
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
    }
}

