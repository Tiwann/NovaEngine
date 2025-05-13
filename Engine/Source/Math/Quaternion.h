#pragma once
#include "Runtime/Types.h"
#include <CoreExport.h>

namespace Nova
{
    struct Vector3;
    struct Vector4;
    struct Matrix4;
    struct Matrix3;

    struct CORE_API Quaternion
    {
        f32 w, x, y, z;

        Quaternion();
        Quaternion(f32 W, f32 X, f32 Y, f32 Z);
        Quaternion(const Quaternion& Other);
        
        f32 Magnitude() const;
        Quaternion Normalized() const;
        Quaternion Conjugated() const;
        Quaternion Inverted() const;

        Quaternion Cross(const Quaternion& Other) const;
        f32 Dot(const Quaternion& Other) const;

        bool operator==(const Quaternion& Other) const;
        bool operator!=(const Quaternion& Other) const;
        Quaternion operator+(const Quaternion& Other) const;
        Quaternion operator-(const Quaternion& Other) const;
        Quaternion& operator+=(const Quaternion& Other);
        Quaternion& operator-=(const Quaternion& Other);
        Quaternion operator*(const Quaternion& Other) const;
        Quaternion operator*(f32 Other) const;
        Quaternion& operator*=(const Quaternion& Other);
        Quaternion& operator*=(f32 Other);
        Quaternion operator/(f32 Other) const;
        Vector3 operator*(const Vector3& Other) const;
        Vector4 operator*(const Vector4& Other) const;
        Matrix4 operator*(const Matrix4& Other) const;

        Matrix4 ToMatrix4() const;
        Matrix3 ToMatrix3() const;
        Vector3 ToEuler() const;
        Vector3 ToEulerDegrees() const;

        static Quaternion FromEuler(const Vector3& EulerAngles);
        static Quaternion FromEuler(f32 X, f32 Y, f32 Z);
        static Quaternion FromEulerDegrees(const Vector3& EulerAnglesDegrees);
        static Quaternion FromEulerDegrees(f32 X, f32 Y, f32 Z);
        static Quaternion One;
        static Quaternion Identity;
    };
}

