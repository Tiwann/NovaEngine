#include "BoxComponent2D.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Physics/BoxShape2D.h"
#include "Physics/PhysicsBody2D.h"
#include "Physics/PhysicsWorld2D.h"
#include "Rendering/DebugRenderer.h"
#include "Runtime/Scene.h"

namespace Nova
{
    void BoxComponent2D::OnInit()
    {
        const Scene* scene = GetScene();
        Ref<PhysicsWorld2D> world = scene->GetPhysicsWorld2D();
        m_Body = world->CreateBody(PhysicsBodyDefinition());
        m_Shape = new BoxShape2D();
        ((PhysicsBody2D*)m_Body)->AttachShape((BoxShape2D*)m_Shape);
        m_Body->SetUserPointer(this);
        m_Body->onContactBeginEvent.BindMember(this, &BoxComponent2D::OnContactBegin);
        m_Body->onContactStayEvent.BindMember(this, &BoxComponent2D::OnContactStay);
        m_Body->onContactEndEvent.BindMember(this, &BoxComponent2D::OnContactEnd);
    }

    void BoxComponent2D::OnDrawDebug()
    {
        const BoxShape2D* shape = (BoxShape2D*)m_Shape;
        const Transform* transform = GetTransform();
        const Vector3 position = transform->GetPosition() + Vector3(shape->GetPosition());
        const Vector3 size = Vector3(shape->GetWidth(), shape->GetHeight(), 0.0f);

        const Quaternion rotation = Quaternion::FromAxisAngle(Vector3::Forward, shape->GetRotation());
        DebugRenderer::DrawSquare(position, rotation, size, Color::Yellow);
    }


    void BoxComponent2D::OnDestroy()
    {
        m_Body->onContactBeginEvent.ClearAll();
        m_Body->onContactStayEvent.ClearAll();
        m_Body->onContactEndEvent.ClearAll();

        const Scene* scene = GetScene();
        Ref<PhysicsWorld2D> world = scene->GetPhysicsWorld2D();
        ((PhysicsBody2D*)m_Body)->DetachShape((BoxShape2D*)m_Shape);
        delete m_Shape;
        world->DestroyBody(m_Body);
    }

    void BoxComponent2D::SetShapePosition(const Vector2& position)
    {
        BoxShape2D* shape = (BoxShape2D*)m_Shape;
        shape->SetPosition(position);
    }

    Vector2 BoxComponent2D::GetShapePosition() const
    {
        const BoxShape2D* shape = (BoxShape2D*)m_Shape;
        return shape->GetPosition();
    }

    void BoxComponent2D::SetShapeRotation(const float radians)
    {
        BoxShape2D* shape = (BoxShape2D*)m_Shape;
        shape->SetRotation(radians);
    }

    float BoxComponent2D::GetShapeRotation() const
    {
        const BoxShape2D* shape = (BoxShape2D*)m_Shape;
        return shape->GetRotation();
    }

    void BoxComponent2D::SetWidth(const float width)
    {
        BoxShape2D* shape = (BoxShape2D*)m_Shape;
        shape->SetWidth(width);
    }

    void BoxComponent2D::SetHeight(const float height)
    {
        BoxShape2D* shape = (BoxShape2D*)m_Shape;
        shape->SetHeight(height);
    }

    void BoxComponent2D::SetSize(const float width, const float height)
    {
        BoxShape2D* shape = (BoxShape2D*)m_Shape;
        shape->SetWidth(Math::Max(width, Math::Epsilon));
        shape->SetHeight(Math::Max(height, Math::Epsilon));
    }

    void BoxComponent2D::OnContactBegin(const PhysicsContact& contact)
    {

    }

    void BoxComponent2D::OnContactStay(const PhysicsContact& contact)
    {
    }

    void BoxComponent2D::OnContactEnd(const PhysicsContact& contact)
    {
    }

    float BoxComponent2D::GetWidth() const
    {
        const BoxShape2D* shape = (BoxShape2D*)m_Shape;
        return shape->GetWidth();
    }

    float BoxComponent2D::GetHeight() const
    {
        const BoxShape2D* shape = (BoxShape2D*)m_Shape;
        return shape->GetHeight();
    }
}
