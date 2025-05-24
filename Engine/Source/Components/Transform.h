#pragma once
#include "Runtime/Component.h"
#include "Math/LinearAlgebra.h"
#include "Runtime/LogCategory.h"
#include "Containers/Lazy.h"
#include "Containers/MulticastDelegate.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(Transform, "TRANSFORM")

namespace Nova
{
    class Transform : public Component
    {
    public:
        Transform(Entity* Owner);
        
        const Vector3& GetPosition() const;
        const Quaternion& GetRotation() const;
        const Vector3& GetScale() const;
        Vector3 GetLocalPosition();
        Vector3 GetLocalRotation();
        Vector3 GetLocalScale();
        
        void SetPosition(const Vector3& Position);
        void SetRotation(const Quaternion& Rotation);
        void SetScale(const Vector3& Scale);
        void SetScale(f32 UniformScale);

        void SetLocalPosition(const Vector3& Position);
        void SetLocalRotation(const Vector3& Rotation);
        void SetLocalScale(const Vector3& Scale);
        void SetLocalScale(f32 UniformScale);
        
        void Translate(const Vector3& Translation);
        void Translate(f32 X, f32 Y, f32 Z);
        void Rotate(const Quaternion& Rotation);
        void RotateAround(const Vector3& EulerAngles, const Vector3& Point);
        void Scale(const Vector3& Vector);
        void Scale(f32 Scalar);

        Vector3 GetForwardVector() const;
        Vector3 GetRightVector() const;
        Vector3 GetUpVector() const;

        const Matrix4& GetWorldSpaceMatrix();
        const Matrix4& GetLocalSpaceMatrix();
        const Matrix3& GetWorldSpaceNormalMatrix();
        void OnInspectorGUI(const ImGuiIO& IO) override;

        MulticastDelegate<void()> OnChanged;
    private:
        Vector3 m_Position = Vector3::Zero;
        Quaternion m_Rotation = Quaternion::Identity;
        Vector3 m_Scale = Vector3::One;

        Lazy<Matrix4> m_WorldSpaceMatrix = Lazy(Matrix4::Identity);
        Lazy<Matrix4> m_LocalSpaceMatrix = Lazy(Matrix4::Identity);
        Lazy<Matrix3> m_WorldSpaceNormalMatrix = Lazy(Matrix3::Identity);
    };

    
}
