#include "PlaneComponent2D.h"

#include "Components/Transform.h"
#include "Physics/PhysicsBody2D.h"
#include "Physics/PhysicsWorld2D.h"
#include "Physics/PlaneShape2D.h"
#include "Runtime/Scene.h"
#include "Rendering/DebugRenderer.h"

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
