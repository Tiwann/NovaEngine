#pragma once
#include "Runtime/Component.h"
#include "Math/LinearAlgebra.h"
#include "Containers/Lazy.h"
#include "Containers/MulticastDelegate.h"

namespace Nova
{
    class Transform : public Component
    {
    public:
        Transform(Entity* owner);
        
        const Vector3& GetPosition() const;
        const Quaternion& GetRotation() const;
        const Vector3& GetScale() const;
        Vector3 GetLocalPosition();
        Vector3 GetLocalRotation();
        Vector3 GetLocalScale();
        
        void SetPosition(const Vector3& position);
        void SetRotation(const Quaternion& rotation);
        void SetScale(const Vector3& scale);
        void SetScale(float scale);

        void SetLocalPosition(const Vector3& localPosition);
        void SetLocalRotation(const Vector3& localRotation);
        void SetLocalScale(const Vector3& localScale);
        void SetLocalScale(float localScale);
        
        void Translate(const Vector3& translation);
        void Rotate(const Quaternion& rotation);
        void RotateAround(const Vector3& eulerAngles, const Vector3& point);
        void Scale(const Vector3& scale);
        void Scale(float scale);

        Vector3 GetForwardVector() const;
        Vector3 GetRightVector() const;
        Vector3 GetUpVector() const;

        const Matrix4& GetWorldSpaceMatrix();
        const Matrix4& GetLocalSpaceMatrix();
        const Matrix3& GetWorldSpaceNormalMatrix();
        //void OnInspectorGUI(const ImGuiIO& IO) override;

        MulticastDelegate<void()> onChanged;
    private:
        Vector3 m_Position = Vector3::Zero;
        Quaternion m_Rotation = Quaternion::Identity;
        Vector3 m_Scale = Vector3::One;

        Lazy<Matrix4> m_WorldSpaceMatrix = Lazy(Matrix4::Identity);
        Lazy<Matrix4> m_LocalSpaceMatrix = Lazy(Matrix4::Identity);
        Lazy<Matrix3> m_WorldSpaceNormalMatrix = Lazy(Matrix3::Identity);
    };

    
}
