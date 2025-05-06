#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Functions.h"

namespace Nova
{
    Vector3 Vector3::Zero     = { 0.0f, 0.0f, 0.0f };
    Vector3 Vector3::One      = { 1.0f, 1.0f, 1.0f };
    Vector3 Vector3::Right    = { 1.0f, 0.0f, 0.0f };
    Vector3 Vector3::Up       = { 0.0f, 1.0f, 0.0f };
    Vector3 Vector3::Left     = { -1.0f, 0.0f, 0.0f };
    Vector3 Vector3::Down     = { 0.0f, -1.0f, 0.0f };
    Vector3 Vector3::Forward  = { 0.0f, 0.0f, 1.0f };
    Vector3 Vector3::Backward = { 0.0f, 0.0f, -1.0f };
    

    Vector3::Vector3(const f32 X, const f32 Y, const f32 Z): x(X), y(Y), z(Z)
    {
    }

    Vector3::Vector3(const f32 Value): x(Value), y(Value), z(Value)
    {
    }

    Vector3::Vector3(const Vector2& Vec): x(Vec.x), y(Vec.y), z(0.0f)
    {
    }

    Vector3::Vector3(const Vector2& Vec, const f32 Z) : x(Vec.x), y(Vec.y), z(Z)
    {
    }

    Vector3::Vector3(const Vector4& Vec) : x(Vec.x), y(Vec.y), z(Vec.z)
    {
    }

    f32 Vector3::Magnitude() const
    {
        return Math::Sqrt(x * x + y * y + z*z);
    }

    f32* Vector3::ValuePtr()
    {
        return (f32*)this;
    }

    const f32* Vector3::ValuePtr() const
    {
        return (const f32*)this;
    }

    f32 Vector3::Dot(const Vector3& Vec) const
    {
        return x * Vec.x + y * Vec.y + z * Vec.z;
    }

    Vector3 Vector3::Cross(const Vector3& Vec) const
    {
        return { y * Vec.z - z * Vec.y,
                    z * Vec.x - x * Vec.z,
                    x * Vec.y - y * Vec.x };
    }

    Vector3 Vector3::WithX(f32 X) const
    {
        return {X, y, z};
    }

    Vector3 Vector3::WithY(f32 Y) const
    {
        return {x, Y, z};
    }

    Vector3 Vector3::WithZ(f32 Z) const
    {
        return {x, y, Z};
    }

    Vector3 Vector3::Normalized() const
    {
        return {x / Magnitude(), y / Magnitude(), z / Magnitude()};
    }

    Vector3 Vector3::operator+(const Vector3& Vec) const
    {
        return {x + Vec.x, y + Vec.y, z + Vec.z};
    }

    Vector3 Vector3::operator-(const Vector3& Vec) const
    {
        return {x - Vec.x, y - Vec.y, z - Vec.z};
    }

    Vector3 Vector3::operator-() const
    {
        return {-x, -y, -z};
    }

    Vector3& Vector3::operator+=(const Vector3& Vec)
    {
        x += Vec.x;
        y += Vec.y;
        z += Vec.z;
        return *this;
    }

    Vector3& Vector3::operator-=(const Vector3& Vec)
    {
        x -= Vec.x;
        y -= Vec.y;
        z -= Vec.z;
        return *this;
    }

    Vector3 Vector3::Apply(f32 (*Function)(f32)) const
    {
        return {Function(x), Function(y), Function(z)};
    }


    Vector3 operator/(const f32 Scalar, const Vector3& Vec)
    {
        return {Scalar / Vec.x, Scalar / Vec.y, Scalar / Vec.z};
    }

    Vector3 operator/(const Vector3& Vec, const f32 Scalar)
    {
        return {Vec.x / Scalar, Vec.y / Scalar, Vec.z / Scalar};
    }

    Vector3 operator*(const f32 Scalar, const Vector3& Vec)
    {
        return {Vec.x * Scalar, Vec.y * Scalar, Vec.z * Scalar};
    }

    Vector3 operator*(const Vector3& Vec, const f32 Scalar)
    {
        return {Vec.x * Scalar, Vec.y * Scalar, Vec.z * Scalar};
    }

    Vector3& Vector3::operator*=(const f32 Scalar)
    {
        x *= Scalar;
        y *= Scalar;
        z *= Scalar;
        return *this;
    }

    Vector3& Vector3::operator*=(const Vector3& Vec)
    {
        x *= Vec.x;
        y *= Vec.y;
        z *= Vec.z;
        return *this;
    }

    Vector3& Vector3::operator/=(const f32 Scalar)
    {
        x /= Scalar;
        y /= Scalar;
        z /= Scalar;
        return *this;
    }

    Vector3& Vector3::operator/=(const Vector3& Vec)
    {
        x /= Vec.x;
        y /= Vec.y;
        z /= Vec.z;
        return *this;
    }

    bool Vector3::operator==(const Vector3& Vec) const
    {
        return Math::AreSame(x, Vec.x)
            && Math::AreSame(y, Vec.y)
            && Math::AreSame(z, Vec.z);
    }

    Vector3 Vector3::operator*(const Vector3& Other) const
    {
        return {x * Other.x, y * Other.y, z * Other.z};
    }

    Vector3 Vector3::Cross(const Vector3& Vec1, const Vector3& Vec2)
    {
        return Vec1.Cross(Vec2);
    }

    Vector3 Vector3::Normalize(const Vector3& Vec)
    {
        return Vec.Normalized();
    }

    f32 Vector3::Dot(const Vector3& Vec1, const Vector3& Vec2)
    {
        return Vec1.Dot(Vec2);
    }

    Vector3 Vector3::Lerp(const Vector3& VecA, const Vector3& VecB, const f32 Alpha)
    {
        return VecA + VecB * Alpha - VecA * Alpha;
    }
    

    f32 Vector3::Angle(const Vector3& VecA, const Vector3& VecB)
    {
        const f32 CosAngle = VecA.Dot(VecB) / (VecA.Magnitude() * VecB.Magnitude());
        return Math::Acos(CosAngle);
    }

    Vector3 Vector3::MoveTowards(const Vector3& Current, const Vector3& Target, const f32 MaxDelta)
    {
        const Vector3 Direction = Target - Current;
        const f32 Distance = Direction.Magnitude();
        const Vector3 MovedVector = Current + Direction / Distance * MaxDelta;
        if (Distance <= MaxDelta || Math::IsZero(Distance))
            return Target;
        return MovedVector;
    }

    Vector3 Vector3::SmoothDamp(const Vector3& Current, const Vector3& Target, Vector3& CurrentVelocity,
        const f32 SmoothTime, const f32 Delta, const f32 MaxSpeed)
    {
        const f32 x = Math::SmoothDamp(Current.x, Target.x, CurrentVelocity.x, SmoothTime, Delta, MaxSpeed);
        const f32 y = Math::SmoothDamp(Current.y, Target.y, CurrentVelocity.y, SmoothTime, Delta, MaxSpeed);
        const f32 z = Math::SmoothDamp(Current.z, Target.z, CurrentVelocity.z, SmoothTime, Delta, MaxSpeed);
        return {x, y, z};
    }
}
