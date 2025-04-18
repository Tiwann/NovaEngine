#pragma once
#include "Runtime/Types.h"
#include <CoreExport.h>

namespace Nova
{
    struct Vector2;
    struct Vector4;

    struct CORE_API Vector3
    {
        union{ f32 x = 0.0f, r; };
        union{ f32 y = 0.0f, g; };
        union{ f32 z = 0.0f, b; };

        constexpr Vector3() = default;
        Vector3(f32 X, f32 Y, f32 Z);
        Vector3(f32 Value);
        Vector3(const Vector2& Vec);
        Vector3(const Vector2& Vec, f32 Z);
        Vector3(const Vector4& Vec);

        f32 Magnitude() const;
        f32* ValuePtr();
        const f32* ValuePtr() const;
        f32 Dot(const Vector3& Vec) const;
        Vector3 Cross(const Vector3& Vec) const;

        Vector3 WithX(f32 X) const;
        Vector3 WithY(f32 Y) const;
        Vector3 WithZ(f32 Z) const;
        Vector3 Normalized() const;
        Vector3 operator+(const Vector3& Vec) const;
        Vector3 operator-(const Vector3& Vec) const;
        Vector3 operator-() const;
        Vector3& operator+=(const Vector3& Vec);
        Vector3& operator-=(const Vector3& Vec);

        Vector3 Apply(f32 (*Function)(f32)) const;

        friend Vector3 operator/(f32 Scalar, const Vector3& Vec);
        friend Vector3 operator/( const Vector3& Vec, f32 Scalar);
        friend Vector3 operator*(f32 Scalar, const Vector3& Vec);
        friend Vector3 operator*(const Vector3& Vec, f32 Scalar);
        
        Vector3& operator*=(f32 Scalar);
        Vector3& operator*=(const Vector3& Vec);

        Vector3& operator/=(f32 Scalar);
        Vector3& operator/=(const Vector3& Vec);
        
        bool operator==(const Vector3& Vec) const;
        Vector3 operator*(const Vector3& Other) const;

        static Vector3 Lerp(const Vector3& VecA, const Vector3& VecB, f32 Alpha);
        static f32 Angle(const Vector3& VecA, const Vector3& VecB);
        static Vector3 MoveTowards(const Vector3& Current, const Vector3& Target, f32 MaxDelta);
        static Vector3 SmoothDamp(const Vector3& Current, const Vector3& Target, Vector3& CurrentVelocity, f32 SmoothTime, f32 Delta, f32 MaxSpeed);
        
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
