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
        Quaternion(float w, float x, float y, float z);
        Quaternion(const Quaternion& other);
        
        float Magnitude() const;
        Quaternion Normalized() const;
        Quaternion Conjugated() const;
        Quaternion Inverted() const;

        Quaternion Cross(const Quaternion& other) const;
        float Dot(const Quaternion& other) const;

        bool operator==(const Quaternion& other) const;
        bool operator!=(const Quaternion& other) const;
        Quaternion operator+(const Quaternion& other) const;
        Quaternion operator-(const Quaternion& other) const;
        Quaternion& operator+=(const Quaternion& other);
        Quaternion& operator-=(const Quaternion& other);
        Quaternion operator*(const Quaternion& other) const;
        Quaternion operator*(float other) const;
        Quaternion& operator*=(const Quaternion& other);
        Quaternion& operator*=(float other);
        Quaternion operator/(float other) const;
        Vector3 operator*(const Vector3& other) const;
        Vector4 operator*(const Vector4& other) const;
        Matrix4 operator*(const Matrix4& other) const;

        Matrix4 ToMatrix4() const;
        Matrix3 ToMatrix3() const;
        Vector3 ToEuler() const;
        Vector3 ToEulerDegrees() const;

        static Quaternion FromAxisAngle(const Vector3& axis, float radians);
        static Quaternion FromAxisAngleDegrees(const Vector3& axis, float degrees);
        static Quaternion FromEuler(const Vector3& eulerAngles);
        static Quaternion FromEulerDegrees(const Vector3& eulerAnglesDegrees);
        static Quaternion One;
        static Quaternion Identity;
    };
}

