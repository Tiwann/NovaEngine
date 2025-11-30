#include "Transform.h"
#include "Runtime/Entity.h"
#include "Runtime/Assertion.h"
#include "Physics/PhysicsComponent.h"
#include <imgui.h>


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
        const Function<Matrix4()> computeWorldSpaceMatrix = [&] -> Matrix4
        {
            const Matrix4& localMatrix = GetLocalSpaceMatrix();

            if(m_Entity && m_Entity->HasParent())
            {
                const Entity* parent = m_Entity->GetParent();
                Transform* parentTransform = parent->GetTransform();
                const Matrix4& parentWorldMatrix = parentTransform->GetWorldSpaceMatrix();
                return parentWorldMatrix * localMatrix;
            }
            return localMatrix;
        };

        return m_WorldSpaceMatrix.Get(computeWorldSpaceMatrix);
    }

    const Matrix4& Transform::GetLocalSpaceMatrix()
    {
        const Function<Matrix4()> computeLocalSpaceMatrix = [&] -> Matrix4
        {
            return Matrix4::TRS(m_Position, m_Rotation, m_Scale);
        };
        return m_LocalSpaceMatrix.Get(computeLocalSpaceMatrix);
    }

    const Matrix3& Transform::GetWorldSpaceNormalMatrix()
    {
        const Function<Matrix3()> computeLocalToWorldNormalMatrix = [&] -> Matrix3
        {
            const Matrix4& modelMatrix = GetWorldSpaceMatrix();
            return Math::Transpose(Math::Inverse(Matrix3(modelMatrix)));
        };

        return m_WorldSpaceNormalMatrix.Get(computeLocalToWorldNormalMatrix);
    }

    void Transform::OnGui()
    {
        Component::OnGui();


        if(ImGui::DragFloat3("Position", m_Position.ValuePtr(), 0.01f, 0, 0, "%.3f"))
        {
            if(PhysicsComponent* physics = m_Entity->GetComponent<PhysicsComponent>())
            {
                physics->SetBodyPosition(m_Position);
            }

            m_WorldSpaceMatrix.SetDirty();
            m_LocalSpaceMatrix.SetDirty();
            m_WorldSpaceNormalMatrix.SetDirty();
            OnChanged.BroadcastChecked();
        }

        // This creates a gimbal lock bug
        Vector3 eulerAngles = m_Rotation.ToEulerDegrees();
        if(ImGui::DragFloat3("Rotation", eulerAngles.ValuePtr(), 0.01f, 0, 0, "%.3f"))
        {
            m_Rotation = Quaternion::FromEulerDegrees(eulerAngles);
            if(PhysicsComponent* physics = m_Entity->GetComponent<PhysicsComponent>())
            {
                physics->SetBodyRotation(m_Rotation);
            }

            m_WorldSpaceMatrix.SetDirty();
            m_LocalSpaceMatrix.SetDirty();
            m_WorldSpaceNormalMatrix.SetDirty();
            OnChanged.BroadcastChecked();
        }

        if(ImGui::DragFloat3("Scale", m_Scale.ValuePtr(), 0.01f, 0, 0, "%.2f"))
        {
            m_WorldSpaceMatrix.SetDirty();
            m_LocalSpaceMatrix.SetDirty();
            m_WorldSpaceNormalMatrix.SetDirty();
            OnChanged.BroadcastChecked();
        }
    }
}

