#include "Transform.h"
#include "Game/Entity.h"
#include "Runtime/Assertion.h"
//#include "Physics/PhysicsComponent.h"
//#include "Physics/RigidBody2D.h"


namespace Nova
{
    Transform::Transform(Entity* owner) : Component(owner, "Transform")
    {
    }

    const Vector3& Transform::GetPosition() const
    {
        return m_Position;
    }

    const Quaternion& Transform::GetRotation() const
    {
        return m_Rotation;
    }

    const Vector3& Transform::GetScale() const
    {
        return m_Scale;
    }

    Vector3 Transform::GetLocalPosition()
    {
        NOVA_ASSERT(false, "Transform::GetLocalPosition() not implemented. Hint: Decompose matrix and return position component.");
        return {};
    }

    Vector3 Transform::GetLocalRotation()
    {
        NOVA_ASSERT(false, "Transform::GetLocalRotation() not implemented. Hint: Decompose matrix and return rotation component.");
        return {};
    }

    Vector3 Transform::GetLocalScale()
    {
        NOVA_ASSERT(false, "Transform::GetLocalScale() not implemented. Hint: Decompose matrix and return scale component.");
        return {};
    }

    void Transform::SetLocalPosition(const Vector3& localPosition)
    {
        NOVA_ASSERT(false, "Transform::GetLocalPosition() not implemented. Hint: Decompose matrix and return position component.");
    }

    void Transform::SetLocalRotation(const Vector3& localRotation)
    {
        NOVA_ASSERT(false, "Transform::GetLocalRotation() not implemented. Hint: Decompose matrix and return rotation component.");
    }

    void Transform::SetLocalScale(const Vector3& localScale)
    {
        NOVA_ASSERT(false, "Transform::GetLocalScale() not implemented. Hint: Decompose matrix and return scale component.");
    }

    void Transform::SetLocalScale(const float localScale)
    {
        NOVA_ASSERT(false, "Transform::GetLocalScale() not implemented. Hint: Decompose matrix and return scale component.");
    }

    void Transform::SetPosition(const Vector3& position)
    {
        m_Position = position;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::SetScale(const float scale)
    {
        SetScale(Vector3(scale));
    }

    void Transform::SetRotation(const Quaternion& rotation)
    {
        m_Rotation = rotation;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::SetScale(const Vector3& scale)
    {
        m_Scale = scale;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    // TODO: We dont want to change transform when controlled by physics
    void Transform::Translate(const Vector3& translation)
    {
        m_Position += translation;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::Rotate(const Quaternion& rotation)
    {
        m_Rotation = rotation * m_Rotation;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }


    void Transform::RotateAround(const Vector3& eulerAngles, const Vector3& point)
    {
        Matrix4 Rotation = Matrix4::Identity;
        Rotation.Translate(point - m_Position);
        Rotation.Rotate(eulerAngles);
        Rotation.Translate(-point + m_Position);

        m_Position = Rotation * m_Position;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::Scale(const Vector3& scale)
    {
        m_Scale *= scale;
        m_WorldSpaceMatrix.SetDirty();
        m_LocalSpaceMatrix.SetDirty();
        OnChanged.BroadcastChecked();
    }

    void Transform::Scale(const float scale)
    {
        m_Scale *= scale;
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
            const Matrix4& LocalMatrix = GetLocalSpaceMatrix();

            if(m_Entity && m_Entity->HasParent())
            {
                const Entity* Parent = m_Entity->GetParent();
                Transform* ParentTransform = Parent->GetTransform();
                const Matrix4& ParentWorldMatrix = ParentTransform->GetWorldSpaceMatrix();
                return ParentWorldMatrix * LocalMatrix;
            }
            return LocalMatrix;
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

    const Matrix3& Transform::GetWorldSpaceNormalMatrix()
    {
        const Function<Matrix3()> ComputeLocalToWorldNormalMatrix = [&]() -> Matrix3
        {
            const Matrix4& ModelMatrix = GetWorldSpaceMatrix();
            return Math::Transpose(Math::Inverse(Matrix3(ModelMatrix)));
        };

        return m_WorldSpaceNormalMatrix.Get(ComputeLocalToWorldNormalMatrix);
    }

    /*void Transform::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);

        if(UI::DragVector3<float>("Position", m_Position, 0.01f, 0, 0, "%.2f"))
        {
            if(PhysicsComponent* Shape = m_Entity->GetComponent<PhysicsComponent>())
            {
                Shape->SetPosition(m_Position);
                //Shape->RecreatePhysicsState();
            }

            m_WorldSpaceMatrix.SetDirty();
            m_LocalSpaceMatrix.SetDirty();
            OnChanged.BroadcastChecked();
        }

        // This creates a gimbal lock bug
        Vector3 EulerAngles = m_Rotation.ToEulerDegrees();
        if(UI::DragVector3<float>("Rotation", EulerAngles, 0.01f, 0, 0, "%.2f"))
        {
            m_Rotation = Quaternion::FromEulerDegrees(EulerAngles);

            if(RigidBody2D* Shape = m_Entity->GetComponent<RigidBody2D>())
            {
                Shape->SetRotation(Quaternion::FromEuler(0.0f, 0.0f, EulerAngles.z));
                //Shape->RecreatePhysicsState();
            }

            m_WorldSpaceMatrix.SetDirty();
            m_LocalSpaceMatrix.SetDirty();
            OnChanged.BroadcastChecked();
        }

        if(UI::DragVector3<float>("Scale", m_Scale, 0.01f, 0, 0, "%.2f"))
        {
            if(RigidBody2D* Shape = m_Entity->GetComponent<RigidBody2D>())
            {
                Shape->RecreatePhysicsState();
            }

            m_WorldSpaceMatrix.SetDirty();
            m_LocalSpaceMatrix.SetDirty();
            OnChanged.BroadcastChecked();
        }
    }*/
}

