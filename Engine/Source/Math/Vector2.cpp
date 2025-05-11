#include "Vector2.h"
#include "Vector3.h"
#include "Functions.h"
#include "Runtime/Assertion.h"

namespace Nova
{
    Vector2 Vector2::Zero    = { 0.0f, 0.0f };
    Vector2 Vector2::One     = { 1.0f, 1.0f };
    Vector2 Vector2::Right   = { 1.0f, 0.0f };
    Vector2 Vector2::Up      = { 0.0f, 1.0f };
    Vector2 Vector2::Left    = { -1.0f, 0.0f };
    Vector2 Vector2::Down    = { 0.0f, -1.0f };
    

    Vector2::Vector2(const f32 X, const f32 Y): x(X), y(Y)
    {
    }

    Vector2::Vector2(const f32 Value): x(Value), y(Value)
    {
    }

    Vector2::Vector2(const Vector3& Vec) : x(Vec.x), y(Vec.y)
    {
    }

    f32 Vector2::Magnitude() const
    {
        return Math::Sqrt(x * x + y * y);
    }

    f32* Vector2::ValuePtr()
    {
        return (f32*)this;
    }

    const f32* Vector2::ValuePtr() const
    {
        return (const f32*)this;
    }

    f32 Vector2::Dot(const Vector2& Vec) const
    {
        return x * Vec.x + y * Vec.y;
    }

    f32 Vector2::Dot(const Vector2& VecA, const Vector2& VecB)
    {
        return VecA.Dot(VecB);
    }

    Vector2 Vector2::Normalize(const Vector2& Vec)
    {
        return Vec.Normalized();
    }

    f32 Vector2::Angle(const Vector2& VecA, const Vector2& VecB)
    {
        return Math::Acos(VecA.Dot(VecB) / (VecA.Magnitude() * VecB.Magnitude()));
    }

    Vector2 Vector2::Lerp(const Vector2& A, const Vector2& B, const f32 Alpha)
    {
        return A + B * Alpha - A * Alpha;
    }

    Vector2 Vector2::QuadraticBezier(const Vector2& A, const Vector2& B, const Vector2& C, const f32 Alpha)
    {
        const Vector2 P0 = Lerp(A, B, Alpha);
        const Vector2 P1 = Lerp(B, C, Alpha);
        return Lerp(P0, P1, Alpha);
    }

    Vector2 Vector2::CubicBezier(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, const f32 Alpha)
    {
        const Vector2 P0 = QuadraticBezier(A, B, C, Alpha);
        const Vector2 P1 = QuadraticBezier(B, C, D, Alpha);
        return Lerp(P0, P1, Alpha);
    }

    Vector2 Vector2::WithX(f32 X) const
    {
        return {X, y};
    }

    Vector2 Vector2::WithY(f32 Y) const
    {
        return {x, Y};
    }

    Vector2 Vector2::Normalized() const
    {
        return {x / Magnitude(), y / Magnitude()};
    }

    Vector2 Vector2::operator+(const Vector2& Vec) const
    {
        return {x + Vec.x, y + Vec.y};
    }

    Vector2 Vector2::operator-(const Vector2& Vec) const
    {
        return {x - Vec.x, y - Vec.y};
    }

    Vector2 Vector2::operator-() const
    {
        return {-x, -y};
    }

    Vector2 operator*(const f32 Scalar, const Vector2& Vec)
    {
        return {Vec.x * Scalar, Vec.y * Scalar};
    }

    Vector2 operator*(const Vector2& Vec, const f32 Scalar)
    {
        return {Vec.x * Scalar, Vec.y * Scalar};
    }

    Vector2 operator/(const Vector2& Vec, const f32 Scalar)
    {
        return { Vec.x / Scalar, Vec.y / Scalar };
    }

    Vector2& Vector2::operator*=(const f32 Scalar)
    {
        x *= Scalar;
        y *= Scalar;
        return *this;
    }

    Vector2& Vector2::operator*=(const Vector2& Scalar)
    {
        x *= Scalar.x;
        y *= Scalar.y;
        return *this;
    }

    Vector2& Vector2::operator/=(const f32 Scalar)
    {
        x /= Scalar;
        y /= Scalar;
        return *this;
    }

    Vector2& Vector2::operator/=(const Vector2& Vec)
    {
        x /= Vec.x;
        y /= Vec.y;
        return *this;
    }

    Vector2& Vector2::operator+=(const Vector2& Vec)
    {
        x *= Vec.x;
        y *= Vec.y;
        return *this;
    }

    Vector2& Vector2::operator-=(const Vector2& Vec)
    {
        x -= Vec.x;
        y -= Vec.y;
        return *this;
    }

    Vector2 Vector2::Apply(f32(* Function)(f32)) const
    {
        NOVA_ASSERT(Function, "Supplied a nullptr function!");
        f32 x = Function(this->x);
        f32 y = Function(this->y);
        return { x, y };
    }

    bool Vector2::operator==(const Vector2& Vec) const
    {
        return Math::AreSame(x, Vec.x)
            && Math::AreSame(y, Vec.y);
    }

    Vector2 Vector2::operator*(const Vector2& Vec) const
    {
        return {x * Vec.x, y * Vec.y};
    }

    f32& Vector2::operator[](const u32 Index)
    {
        return ValuePtr()[Index];
    }

    const f32& Vector2::operator[](const u32 Index) const
    {
        return ValuePtr()[Index];
    }
}
