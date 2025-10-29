#pragma once

#include <cstdint>

namespace Nova
{
    struct Vector2;
    struct Vector4;

    struct Vector3
    {
        union{ float x = 0.0f, r; };
        union{ float y = 0.0f, g; };
        union{ float z = 0.0f, b; };

        constexpr Vector3() = default;
        Vector3(float X, float Y, float Z);
        explicit Vector3(float Value);
        explicit Vector3(const Vector2& Vec);
        Vector3(const Vector2& Vec, float Z);
        explicit Vector3(const Vector4& Vec);

        float Magnitude() const;
        float MagnitudeSquared() const;
        float* ValuePtr();
        const float* ValuePtr() const;
        float Dot(const Vector3& Vec) const;
        Vector3 Cross(const Vector3& Vec) const;

        Vector3 WithX(float X) const;
        Vector3 WithY(float Y) const;
        Vector3 WithZ(float Z) const;
        Vector3 Normalized() const;
        Vector3 Apply(float (*Function)(float)) const;

        Vector3 operator+(const Vector3& Vec) const;
        Vector3 operator-(const Vector3& Vec) const;
        Vector3 operator-() const;
        Vector3& operator+=(const Vector3& Vec);
        Vector3& operator-=(const Vector3& Vec);

        friend Vector3 operator*(float Scalar, const Vector3& Vec);
        friend Vector3 operator*(const Vector3& Vec, float Scalar);
        friend Vector3 operator/(const Vector3& Vec, float Scalar);

        Vector3& operator*=(float Scalar);
        Vector3& operator*=(const Vector3& Vec);

        Vector3& operator/=(float Scalar);
        Vector3& operator/=(const Vector3& Vec);
        
        bool operator==(const Vector3& Vec) const;
        Vector3 operator*(const Vector3& Other) const;

        float& operator[](uint32_t Index);
        const float& operator[](uint32_t Index) const;

        static float Dot(const Vector3& Vec1, const Vector3& Vec2);
        static Vector3 Cross(const Vector3& Vec1, const Vector3& Vec2);
        static Vector3 Normalize(const Vector3& Vec);
        static float Angle(const Vector3& VecA, const Vector3& VecB);
        static Vector3 Lerp(const Vector3& VecA, const Vector3& VecB, float Alpha);
        static Vector3 MoveTowards(const Vector3& Current, const Vector3& Target, float MaxDelta);
        static Vector3 InterpTo(const Vector3& Current, const Vector3& Target, float Speed, float Delta);

        static Vector3 Zero;
        static Vector3 One;
        static Vector3 Right;
        static Vector3 Up;
        static Vector3 Left;
        static Vector3 Down;
        static Vector3 Forward;
        static Vector3 Backward;
    };
}
