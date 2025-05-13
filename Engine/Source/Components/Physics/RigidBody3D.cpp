#include "RigidBody3D.h"
#include "Contact3D.h"
#include "Components/Transform.h"
#include "Runtime/Scene.h"
#include "Math/Vector3.h"
#include "Physics/PhysicsShape3D.h"
#include "Physics/PhysicsBody3D.h"
#include "Editor/EditorGUI.h"

namespace Nova
{
    RigidBody3D::RigidBody3D(Entity* Owner, const String& Name) : PhysicsComponent(Owner, Name)
    {
    }

    void RigidBody3D::OnContactBegin(const PhysicsContactInfo3D& ContactInfo)
    {
        Contact3D Contact;
        Contact.ImpactPoint = ContactInfo.Point;
        Contact.Normal = ContactInfo.Normal;
        !m_PhysicsBody->IsSensor() ? OnContactBeginEvent.Broadcast(Contact) : OnTriggerBeginEvent.Broadcast(Contact);
    }

    void RigidBody3D::OnContactStay(const PhysicsContactInfo3D& ContactInfo)
    {
        Contact3D Contact;
        Contact.ImpactPoint = ContactInfo.Point;
        Contact.Normal = ContactInfo.Normal;
        !m_PhysicsBody->IsSensor() ? OnContactStayEvent.Broadcast(Contact) : OnTriggerStayEvent.Broadcast(Contact);
    }

    void RigidBody3D::OnContactEnd(const PhysicsContactInfo3D& ContactInfo)
    {
        Contact3D Contact;
        Contact.ImpactPoint = ContactInfo.Point;
        Contact.Normal = ContactInfo.Normal;
        !m_PhysicsBody->IsSensor() ? OnContactEndEvent.Broadcast(Contact) : OnTriggerEndEvent.Broadcast(Contact);
    }

    void RigidBody3D::OnInit()
    {
        Component::OnInit();

        Scene* Scene = m_Entity->GetScene();
        PhysicsWorld3D& World = Scene->GetPhysicsWorld3D();
        
        Transform* Transform = GetTransform();
        const Vector3 Position = Transform->GetPosition();
        const Vector3 Rotation = Transform->GetRotation();
        
        const PhysicsBodyDefinition Definition { Position, Rotation, PhysicsBodyType::Static, false };
        m_PhysicsShape = CreateShape(Transform);
        m_PhysicsBody = World.CreateBody(Definition, PhysicsMaterial(), m_PhysicsShape);
        m_PhysicsBody->CreatePhysicsState(m_PhysicsShape, PhysicsMaterial());

        m_PhysicsBody->OnContactBeginEvent.BindMember(this, &RigidBody3D::OnContactBegin);
        m_PhysicsBody->OnContactStayEvent.BindMember(this, &RigidBody3D::OnContactStay);
        m_PhysicsBody->OnContactEndEvent.BindMember(this, &RigidBody3D::OnContactEnd);
    }

    void RigidBody3D::OnStart()
    {
        Component::OnStart();
    }

    void RigidBody3D::OnDestroy()
    {
        PhysicsComponent::OnDestroy();
        
        Scene* Scene = m_Entity->GetScene();
        PhysicsWorld3D& World = Scene->GetPhysicsWorld3D();

        m_PhysicsBody->OnContactBeginEvent.ClearAll();
        m_PhysicsBody->OnContactStayEvent.ClearAll();
        m_PhysicsBody->OnContactEndEvent.ClearAll();

        m_PhysicsBody->DestroyPhysicsState();
        World.DestroyBody(m_PhysicsBody);
        delete m_PhysicsShape;
    }

    void RigidBody3D::OnPhysicsUpdate(f32 Delta)
    {
        Component::OnPhysicsUpdate(Delta);
        Transform* Transform = GetTransform();
        const Vector3 NewPosition = m_PhysicsBody->GetPosition();
        const Vector3 NewRotation = m_PhysicsBody->GetRotation();
        Transform->SetPosition(NewPosition);
        Transform->SetRotation(NewRotation);
    }

    void RigidBody3D::OnInspectorGUI(const ImGuiIO& IO)
    {
        PhysicsComponent::OnInspectorGUI(IO);
        const char* BodyTypes[3] = { "Static", "Kinematic", "Dynamic" };
        PhysicsBodyType BodyType = m_PhysicsBody->GetType();
        if(ImGui::Combo("Collider Type", (int*)&BodyType, BodyTypes, 3))
        {
            m_PhysicsBody->SetType(BodyType);
        }
    }

    void RigidBody3D::SetGravityScale(f32 Scale)
    {
        m_PhysicsBody->SetGravityScale(Scale);
    }

    void RigidBody3D::SetLinearVelocity(const Vector3& Velocity)
    {
        m_PhysicsBody->SetLinearVelocity(Velocity);
    }

    void RigidBody3D::SetAngularVelocity(const Vector3& AngularVelocity)
    {
        m_PhysicsBody->SetAngularVelocity(AngularVelocity);
    }

    void RigidBody3D::SetLinearDamping(f32 LinearDamping)
    {
        m_PhysicsBody->SetLinearDamping(LinearDamping);
    }

    void RigidBody3D::SetAngularDamping(f32 AngularDamping)
    {
        m_PhysicsBody->SetAngularDamping(AngularDamping);
    }

    f32 RigidBody3D::GetGravityScale() const
    {
        return m_PhysicsBody->GetGravityScale();
    }

    Vector3 RigidBody3D::GetLinearVelocity() const
    {
        return m_PhysicsBody->GetLinearVelocity();
    }

    Vector3 RigidBody3D::GetLinearVelocityPoint(const Vector3& Point) const
    {
        return m_PhysicsBody->GetLinearVelocityPoint(Point);
    }

    Vector3 RigidBody3D::GetAngularVelocity() const
    {
        return m_PhysicsBody->GetAngularVelocity();
    }

    f32 RigidBody3D::GetLinearDamping() const
    {
        return m_PhysicsBody->GetLinearDamping();
    }

    f32 RigidBody3D::GetAngularDamping() const
    {
        return m_PhysicsBody->GetAngularDamping();
    }
    
    void RigidBody3D::AddForce(const Vector3& Force)
    {
        m_PhysicsBody->AddForce(Force);
    }

    void RigidBody3D::AddImpulse(const Vector3& Force)
    {
        m_PhysicsBody->AddImpulse(Force);
    }

    void RigidBody3D::AddForceAtPosition(const Vector3& Position, const Vector3& Force)
    {
        m_PhysicsBody->AddForceAtPosition(Position, Force);
    }

    void RigidBody3D::AddImpulseAtPosition(const Vector3& Position, const Vector3& Force)
    {
        m_PhysicsBody->AddImpulseAtPosition(Position, Force);
    }

    void RigidBody3D::RecreatePhysicsState()
    {
        m_PhysicsBody->DestroyPhysicsState();
        m_PhysicsBody->CreatePhysicsState(m_PhysicsShape, m_PhysicsBody->GetMaterial());
    }

    const PhysicsMaterial& RigidBody3D::GetMaterial() const
    {
        return m_PhysicsBody->GetMaterial();
    }

    void RigidBody3D::SetMaterial(const PhysicsMaterial& Material)
    {
        m_PhysicsBody->SetMaterial(Material);
    }

    PhysicsConstraintsFlags RigidBody3D::GetConstraints()
    {
        return m_PhysicsBody->GetConstraints();
    }

    void RigidBody3D::SetConstraints(PhysicsConstraintsFlags Constraints)
    {
        m_PhysicsBody->SetConstraints(Constraints);
    }

    bool RigidBody3D::IsSensor() const
    {
        return m_PhysicsBody->IsSensor();
    }

    void RigidBody3D::SetSensor(bool Sensor)
    {
        m_PhysicsBody->SetIsSensor(Sensor);
    }

    PhysicsBodyType RigidBody3D::GetPhysicsBodyType() const
    {
        return m_PhysicsBody->GetType();
    }

    void RigidBody3D::SetPhysicsBodyType(PhysicsBodyType Type)
    {
        m_PhysicsBody->SetType(Type);
    }

    void RigidBody3D::SetPosition(const Vector3& Position)
    {
        m_PhysicsBody->SetPosition(Position);
    }

    void RigidBody3D::SetRotation(const Quaternion& Rotation)
    {
        m_PhysicsBody->SetRotation(Rotation);
    }
}
