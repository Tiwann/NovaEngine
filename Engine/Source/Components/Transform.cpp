#include "Transform.h"
#include "Runtime/Entity.h"
#include "Editor/EditorGUI.h"
#include "Physics/PhysicsComponent.h"
#include "Physics/RigidBody2D.h"


namespace Nova
{
    Transform::Transform(Entity* Owner) : Component(Owner, "Transform")
    {
    }

    const Vector3& Transform::GetPosition() const
    {
        return m_Position;
    }

    const Vector3& Transform::GetRotation() const
    {
        return m_Rotation;
    }

    const Vector3& Transform::GetScale() const
    {
        return m_Scale;
    }

    Vector3 Transform::GetLocalPosition()
    {
        return {};
    }

    Vector3 Transform::GetLocalRotation()
    {
        return {};
    }

    Vector3 Transform::GetLocalScale()
    {

        return {};
    }

    void Transform::SetLocalPosition(const Vector3& Position)
    {
    }

    void Transform::SetLocalRotation(const Vector3& Rotation)
    {
    }

    void Transform::SetLocalScale(const Vector3& Scale)
    {
    }

    void Transform::SetLocalScale(f32 UniformScale)
    {
    }

    void Transform::SetPosition(const Vector3& Position)
    {
        m_Position = Position;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::SetPosition(f32 X, f32 Y, f32 Z)
    {
        SetPosition({X, Y, Z});
    }

    void Transform::SetRotation(f32 X, f32 Y, f32 Z)
    {
        SetRotation({X, Y, Z});
    }

    void Transform::SetScale(f32 X, f32 Y, f32 Z)
    {
        SetScale({X, Y, Z});
    }

    void Transform::SetScale(const f32 UniformScale)
    {
        SetScale(Vector3(UniformScale));
    }

    void Transform::SetRotation(const Vector3& Rotation)
    {
        m_Rotation = Rotation;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::SetScale(const Vector3& Scale)
    {
        m_Scale = Scale;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::Translate(const Vector3& Translation)
    {
        if(m_Entity->GetComponent<RigidBody2D>() && Translation != Vector3::Zero)
        {
            NOVA_LOG(Transform, Verbosity::Warning, "Tried to translate an object that is controlled by physics.");
        }
        m_Position += Translation;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::Translate(const f32 X, const f32 Y, const f32 Z)
    {
        m_Position += Vector3(X, Y, Z);
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::Rotate(const Vector3& EulerAngles)
    {
        const Vector3 TargetRotation = m_Rotation + EulerAngles;
        m_Rotation = Math::Wrap(TargetRotation, 0.0f, 360.0f);
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    
    void Transform::RotateAround(const Vector3& EulerAngles, const Vector3& Point)
    {
        Matrix4 Rotation = Matrix4::Identity;
        Rotation.Translate(Point - m_Position);
        Rotation.Rotate(EulerAngles);
        Rotation.Translate(-Point + m_Position);

        m_Position = Rotation * m_Position;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::Scale(const Vector3& Vector)
    {
        m_Scale *= Vector;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::Scale(const f32 Scalar)
    {
        m_Scale *= Scalar;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    Vector3 Transform::GetForwardVector() const
    {
        return Math::ForwardFromRotation(m_Rotation);
    }

    Vector3 Transform::GetRightVector() const
    {
        return Math::RightFromRotation(m_Rotation);
    }

    Vector3 Transform::GetUpVector() const
    {
        return Math::UpFromRotation(m_Rotation);
    }

    const Matrix4& Transform::GetWorldSpaceMatrix()
    {
        const Function<Matrix4()> ComputeWorldSpaceMatrix = [&]() -> Matrix4
        {
            if(m_Entity && m_Entity->HasParent())
            {
                return GetLocalSpaceMatrix() * m_Entity->GetParent()->GetTransform()->GetWorldSpaceMatrix() ;
            }
            return GetLocalSpaceMatrix();
        };

        return m_WorldSpaceMatrix.Get(ComputeWorldSpaceMatrix);
    }

    const Matrix4& Transform::GetLocalSpaceMatrix()
    {
        const Function<Matrix4()> ComputeLocalSpaceMatrix = [&]() -> Matrix4
        {
            return Matrix4::TRS(m_Position, m_Rotation, m_Scale);
        };

        return m_LocalSpaceMatrix.Get(ComputeLocalSpaceMatrix);
    }

    void Transform::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);
        
        if(UI::DragVector3<f32>("Position", m_Position, 0.01f, 0, 0, "%.2f"))
        {
            if(RigidBody2D* Shape = m_Entity->GetComponent<RigidBody2D>())
            {
                Shape->SetPosition(m_Position);
                Shape->RecreatePhysicsState();
            }

            m_WorldSpaceMatrix.SetDirty();
            m_LocalSpaceMatrix.SetDirty();
            OnChanged.BroadcastChecked();
        }
        
        if(UI::DragVector3<f32>("Rotation", m_Rotation, 0.01f, 0, 0, "%.2f"))
        {
            if(RigidBody2D* Shape = m_Entity->GetComponent<RigidBody2D>())
            {
                Shape->SetRotation(Vector3(m_Rotation.z));
                Shape->RecreatePhysicsState();
            }

            m_WorldSpaceMatrix.SetDirty();
            m_LocalSpaceMatrix.SetDirty();
            OnChanged.BroadcastChecked();
        }
        
        if(UI::DragVector3<f32>("Scale", m_Scale, 0.01f, 0, 0, "%.2f"))
        {
            if(RigidBody2D* Shape = m_Entity->GetComponent<RigidBody2D>())
            {
                Shape->RecreatePhysicsState();
            }

            m_WorldSpaceMatrix.SetDirty();
            m_LocalSpaceMatrix.SetDirty();
            OnChanged.BroadcastChecked();
        }
    }
}

