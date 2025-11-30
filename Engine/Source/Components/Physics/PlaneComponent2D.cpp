#include "PlaneComponent2D.h"

#include "Components/Transform.h"
#include "Physics/PhysicsBody2D.h"
#include "Physics/PhysicsWorld2D.h"
#include "Physics/PlaneShape2D.h"
#include "Runtime/Scene.h"
#include "Rendering/DebugRenderer.h"
#include <imgui.h>
namespace Nova
{
    void PlaneComponent2D::OnInit()
    {
        const Scene* scene = GetScene();
        Ref<PhysicsWorld2D> world = scene->GetPhysicsWorld2D();
        m_Body = world->CreateBody(PhysicsBodyDefinition());
        m_Shape = new PlaneShape2D();
        ((PhysicsBody2D*)m_Body)->AttachShape((PlaneShape2D*)m_Shape);
        m_Body->SetUserPointer(this);
    }

    void PlaneComponent2D::OnDrawDebug()
    {
        const Transform* transform = GetTransform();
        const PlaneShape2D* shape = (PlaneShape2D*)m_Shape;

        const Quaternion rotation = Quaternion::FromAxisAngle(Vector3::Forward, shape->GetRotation());
        const Vector3 position = transform->GetPosition() + Vector3(shape->GetPosition());
        const Vector3 start = position + rotation * Vector3::Left * shape->GetWidth() * 0.5f;
        const Vector3 end = position + rotation * Vector3::Right * shape->GetWidth() * 0.5f;
        DebugRenderer::DrawLine(start, end, Color::Yellow);
    }

    void PlaneComponent2D::OnDestroy()
    {
        const Scene* scene = GetScene();
        Ref<PhysicsWorld2D> world = scene->GetPhysicsWorld2D();
        ((PhysicsBody2D*)m_Body)->DetachShape((PlaneShape2D*)m_Shape);
        delete m_Shape;
        world->DestroyBody(m_Body);
    }

    void PlaneComponent2D::OnGui()
    {
        Vector2 position = GetShapePosition();
        if (ImGui::DragFloat2("Position", position.ValuePtr(), 0.01f))
        {
            SetShapePosition(position);
        }

        float rotation = GetShapeRotation();
        if (ImGui::DragFloat("Rotation", &rotation, 0.01f))
        {
            SetShapeRotation(rotation);
        }

        float width = GetWidth();
        if (ImGui::DragFloat("Width", &width, 0.01f))
        {
            SetWidth(width);
        }

        float gravityScale = GetGravityScale();
        if (ImGui::DragFloat("Gravity Scale", &gravityScale, 0.01f))
        {
            SetGravityScale(gravityScale);
        }

        float linearDamping = GetLinearDamping();
        if (ImGui::DragFloat("Linear Damping", &linearDamping, 0.01f))
        {
            SetLinearDamping(linearDamping);
        }

        float angularDamping = GetAngularDamping();
        if (ImGui::DragFloat("Angular Damping", &angularDamping, 0.01f))
        {
            SetAngularDamping(angularDamping);
        }

        const char* constraintsFlagNames[]
        {
            "None",
            "PositionX",
            "PositionY",
            "PositionZ",
            "RotationX",
            "RotationY",
            "RotationZ"
        };

        PhysicsConstraintsFlags flags = GetConstraints();

        if(ImGui::BeginCombo("Constraints", nullptr, ImGuiComboFlags_NoPreview))
        {
            for(size_t i = 1; i < std::size(constraintsFlagNames); i++)
            {
                bool isSlected = flags.Contains(PhysicsConstraintsFlagBits(1 << i));
                if(ImGui::Selectable(constraintsFlagNames[i], &isSlected))
                {
                    flags.Toggle(PhysicsConstraintsFlagBits(1 << i));
                }
            }
            SetConstraints(flags);
            ImGui::EndCombo();
        }

        const char* bodyTypeNames[]
        {
            "Static",
            "Kinematic",
            "Dynamic"
        };
        PhysicsBodyType bodyType = GetType();
        if (ImGui::Combo("Body Type", (int*)&bodyType, bodyTypeNames, std::size(bodyTypeNames)))
        {
            SetType(bodyType);
        }
    }

    void PlaneComponent2D::SetShapePosition(const Vector2& position)
    {
        ((PlaneShape2D*)m_Shape)->SetPosition(position);
    }

    void PlaneComponent2D::SetShapeRotation(const float rotation)
    {
        ((PlaneShape2D*)m_Shape)->SetRotation(rotation);
    }

    Vector2 PlaneComponent2D::GetShapePosition() const
    {
        return ((PlaneShape2D*)m_Shape)->GetPosition();
    }

    float PlaneComponent2D::GetShapeRotation()
    {
        return ((PlaneShape2D*)m_Shape)->GetRotation();
    }

    float PlaneComponent2D::GetWidth() const
    {
        return ((PlaneShape2D*)m_Shape)->GetWidth();
    }

    void PlaneComponent2D::SetWidth(const float width)
    {
        ((PlaneShape2D*)m_Shape)->SetWidth(Math::Max(width, Math::Epsilon));
    }
}
