#include "PlaneComponent2D.h"
#include "Physics/PhysicsBody2D.h"
#include "Physics/PhysicsWorld2D.h"
#include "Physics/PlaneShape2D.h"
#include "Runtime/Scene.h"

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

    void PlaneComponent2D::SetWidth(const float width)
    {
        ((PlaneShape2D*)m_Shape)->SetWidth(width);
    }
}
