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

    void Transform::SetScale(const f32 UniformScale)
    {
        SetScale(Vector3(UniformScale));
    }

    void Transform::SetRotation(const Quaternion& Rotation)
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
            return;
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

    void Transform::Rotate(const Quaternion& Rotation)
    {
        m_Rotation = Rotation * m_Rotation;
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

    void Transform::OnInspectorGUI(const ImGuiIO& IO)
    {
        Component::OnInspectorGUI(IO);

        if(UI::DragVector3<f32>("Position", m_Position, 0.01f, 0, 0, "%.2f"))
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
        if(UI::DragVector3<f32>("Rotation", EulerAngles, 0.01f, 0, 0, "%.2f"))
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

