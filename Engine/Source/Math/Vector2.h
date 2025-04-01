#pragma once
#include "Runtime/Types.h"
#include <CoreExport.h>

namespace Nova
{
    struct Vector3;
    
    struct CORE_API Vector2
    {
        union{ f32 x{0.0f}, r; };
        union{ f32 y{0.0f}, g; };

        Vector2() = default;
        Vector2(f32 X, f32 Y);
        Vector2(f32 Value);
        Vector2(const Vector3& Vec);
        
        f32 Magnitude() const;
        f32* ValuePtr();
        f32 Dot(const Vector2& Vec) const;

        static f32 Dot(const Vector2& VecA, const Vector2& VecB);
        static f32 Angle(const Vector2& VecA, const Vector2& VecB);
        static Vector2 Lerp(const Vector2& A, const Vector2& B, f32 Alpha);
        static Vector2 QuadraticBezier(const Vector2& A, const Vector2& B, const Vector2& C, f32 Alpha);
        static Vector2 CubicBezier(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, f32 Alpha);

        Vector2 WithX(f32 X) const;
        Vector2 WithY(f32 Y) const;
        Vector2 Normalized() const;
        Vector2 operator+(const Vector2& Vec) const;
        Vector2 operator-(const Vector2& Vec) const;
        Vector2 operator-() const;

        Vector2 operator/(f32 Scalar) const;
        friend Vector2 operator*(f32 Scalar, const Vector2& Vec);
        friend Vector2 operator*(const Vector2& Vec, f32 Scalar);
        
        Vector2& operator*=(f32 Scalar);
        Vector2& operator+=(const Vector2& Vec);
        bool operator==(const Vector2& Vec) const;
        Vector2 operator*(const Vector2& Vec) const;


        static Vector2 Zero;
        static Vector2 One;
        static Vector2 Right;
        static Vector2 Up;
        static Vector2 Left;
        static Vector2 Down;
    };
}
