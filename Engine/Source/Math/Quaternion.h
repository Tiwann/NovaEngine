#pragma once


namespace Nova
{
    struct Vector3;
    struct Vector4;
    struct Matrix4;
    struct Matrix3;

    struct Quaternion
    {
        float w, x, y, z;

        Quaternion();
        Quaternion(float W, float X, float Y, float Z);
        Quaternion(const Quaternion& Other);
        
        float Magnitude() const;
        Quaternion Normalized() const;
        Quaternion Conjugated() const;
        Quaternion Inverted() const;

        Quaternion Cross(const Quaternion& Other) const;
        float Dot(const Quaternion& Other) const;

        bool operator==(const Quaternion& Other) const;
        bool operator!=(const Quaternion& Other) const;
        Quaternion operator+(const Quaternion& Other) const;
        Quaternion operator-(const Quaternion& Other) const;
        Quaternion& operator+=(const Quaternion& Other);
        Quaternion& operator-=(const Quaternion& Other);
        Quaternion operator*(const Quaternion& Other) const;
        Quaternion operator*(float Other) const;
        Quaternion& operator*=(const Quaternion& Other);
        Quaternion& operator*=(float Other);
        Quaternion operator/(float Other) const;
        Vector3 operator*(const Vector3& Other) const;
        Vector4 operator*(const Vector4& Other) const;
        Matrix4 operator*(const Matrix4& Other) const;

        Matrix4 ToMatrix4() const;
        Matrix3 ToMatrix3() const;
        Vector3 ToEuler() const;
        Vector3 ToEulerDegrees() const;

        static Quaternion FromAxisAngle(const Vector3& Axis, float Radians);
        static Quaternion FromAxisAngleDegrees(const Vector3& Axis, float Degrees);
        static Quaternion FromEuler(const Vector3& EulerAngles);
        static Quaternion FromEuler(float X, float Y, float Z);
        static Quaternion FromEulerDegrees(const Vector3& EulerAnglesDegrees);
        static Quaternion FromEulerDegrees(float X, float Y, float Z);
        static Quaternion One;
        static Quaternion Identity;
    };
}

