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
    

    Vector3::Vector3(const float X, const float Y, const float Z): x(X), y(Y), z(Z)
    {
    }

    Vector3::Vector3(const float Value): x(Value), y(Value), z(Value)
    {
    }

    Vector3::Vector3(const Vector2& Vec): x(Vec.x), y(Vec.y), z(0.0f)
    {
    }

    Vector3::Vector3(const Vector2& Vec, const float Z) : x(Vec.x), y(Vec.y), z(Z)
    {
    }

    Vector3::Vector3(const Vector4& Vec) : x(Vec.x), y(Vec.y), z(Vec.z)
    {
    }

    float Vector3::Magnitude() const
    {
        return Math::Sqrt(x * x + y * y + z * z);
    }

    float Vector3::MagnitudeSquared() const
    {
        return x * x + y * y + z * z;
    }

    float* Vector3::ValuePtr()
    {
        return (float*)this;
    }

    const float* Vector3::ValuePtr() const
    {
        return (const float*)this;
    }

    float Vector3::Dot(const Vector3& Vec) const
    {
        return x * Vec.x + y * Vec.y + z * Vec.z;
    }

    Vector3 Vector3::Cross(const Vector3& Vec) const
    {
        return { y * Vec.z - z * Vec.y,
            z * Vec.x - x * Vec.z,
            x * Vec.y - y * Vec.x };

    }

    Vector3 Vector3::WithX(float X) const
    {
        return {X, y, z};
    }

    Vector3 Vector3::WithY(float Y) const
    {
        return {x, Y, z};
    }

    Vector3 Vector3::WithZ(float Z) const
    {
        return {x, y, Z};
    }

    Vector3 Vector3::Normalized() const
    {
        const float magnitude = Magnitude();
        return {x / magnitude, y / magnitude, z / magnitude};
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

    Vector3 Vector3::Apply(float (*Function)(float)) const
    {
        return {Function(x), Function(y), Function(z)};
    }

    Vector3 operator/(const Vector3& Vec, const float Scalar)
    {
        return {Vec.x / Scalar, Vec.y / Scalar, Vec.z / Scalar};
    }

    Vector3 operator*(const float Scalar, const Vector3& Vec)
    {
        return {Vec.x * Scalar, Vec.y * Scalar, Vec.z * Scalar};
    }

    Vector3 operator*(const Vector3& Vec, const float Scalar)
    {
        return {Vec.x * Scalar, Vec.y * Scalar, Vec.z * Scalar};
    }

    Vector3& Vector3::operator*=(const float Scalar)
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

    Vector3& Vector3::operator/=(const float Scalar)
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

    float& Vector3::operator[](const uint32_t Index)
    {
        return ValuePtr()[Index];
    }

    const float& Vector3::operator[](const uint32_t Index) const
    {
        return ValuePtr()[Index];
    }

    Vector3 Vector3::Cross(const Vector3& Vec1, const Vector3& Vec2)
    {
        return Vec1.Cross(Vec2);
    }

    Vector3 Vector3::Normalize(const Vector3& Vec)
    {
        return Vec.Normalized();
    }

    float Vector3::Dot(const Vector3& Vec1, const Vector3& Vec2)
    {
        return Vec1.Dot(Vec2);
    }

    Vector3 Vector3::Lerp(const Vector3& VecA, const Vector3& VecB, const float Alpha)
    {
        return VecA + Alpha * (VecB - VecA);
    }
    

    float Vector3::Angle(const Vector3& VecA, const Vector3& VecB)
    {
        const float CosAngle = VecA.Dot(VecB) / (VecA.Magnitude() * VecB.Magnitude());
        return Math::Acos(CosAngle);
    }

    Vector3 Vector3::MoveTowards(const Vector3& Current, const Vector3& Target, const float MaxDelta)
    {
        const Vector3 Direction = Target - Current;
        const float Distance = Direction.Magnitude();
        const Vector3 MovedVector = Current + Direction / Distance * MaxDelta;
        if (Distance <= MaxDelta || Math::IsZero(Distance))
            return Target;
        return MovedVector;
    }

    Vector3 Vector3::InterpTo(const Vector3& Current, const Vector3& Target, const float Speed, const float Delta)
    {
        if(Speed <= 0.0f)
            return Target;

        const Vector3 Dist = Target - Current;

        if( Dist.MagnitudeSquared() < Math::Epsilon )
            return Target;

        const Vector3 DeltaMove = Dist * Math::Saturate(Delta * Speed);
        return Current + DeltaMove;
    }
}
