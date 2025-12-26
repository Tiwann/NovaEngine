#include "Vector2.h"
#include "Vector3.h"
#include "Functions.h"
#include "Runtime/Assertion.h"

namespace Nova
{
    const Vector2 Vector2::Zero    = { 0.0f, 0.0f };
    const Vector2 Vector2::One     = { 1.0f, 1.0f };
    const Vector2 Vector2::Right   = { 1.0f, 0.0f };
    const Vector2 Vector2::Up      = { 0.0f, 1.0f };
    const Vector2 Vector2::Left    = { -1.0f, 0.0f };
    const Vector2 Vector2::Down    = { 0.0f, -1.0f };
    

    Vector2::Vector2(const float x, const float y): x(x), y(y)
    {
    }

    Vector2::Vector2(const float value): x(value), y(value)
    {
    }

    Vector2::Vector2(const Vector3& vector) : x(vector.x), y(vector.y)
    {
    }

    float Vector2::Magnitude() const
    {
        return Math::Sqrt(x * x + y * y);
    }

    float* Vector2::ValuePtr()
    {
        return (float*)this;
    }

    const float* Vector2::ValuePtr() const
    {
        return (const float*)this;
    }

    float Vector2::Dot(const Vector2& vector) const
    {
        return x * vector.x + y * vector.y;
    }

    float Vector2::Dot(const Vector2& vectorA, const Vector2& vectorB)
    {
        return vectorA.Dot(vectorB);
    }

    Vector2 Vector2::Normalize(const Vector2& vector)
    {
        return vector.Normalized();
    }

    float Vector2::Angle(const Vector2& vectorA, const Vector2& vectorB)
    {
        return Math::Acos(vectorA.Dot(vectorB) / (vectorA.Magnitude() * vectorB.Magnitude()));
    }

    Vector2 Vector2::Lerp(const Vector2& A, const Vector2& B, const float Alpha)
    {
        return A + B * Alpha - A * Alpha;
    }

    Vector2 Vector2::QuadraticBezier(const Vector2& A, const Vector2& B, const Vector2& C, const float Alpha)
    {
        const Vector2 P0 = Lerp(A, B, Alpha);
        const Vector2 P1 = Lerp(B, C, Alpha);
        return Lerp(P0, P1, Alpha);
    }

    Vector2 Vector2::CubicBezier(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, const float Alpha)
    {
        const Vector2 P0 = QuadraticBezier(A, B, C, Alpha);
        const Vector2 P1 = QuadraticBezier(B, C, D, Alpha);
        return Lerp(P0, P1, Alpha);
    }

    Vector2 Vector2::WithX(float x) const
    {
        return {x, y};
    }

    Vector2 Vector2::WithY(float y) const
    {
        return {x, y};
    }

    Vector2 Vector2::Normalized() const
    {
        return {x / Magnitude(), y / Magnitude()};
    }

    Vector2 Vector2::operator+(const Vector2& vector) const
    {
        return {x + vector.x, y + vector.y};
    }

    Vector2 Vector2::operator-(const Vector2& vector) const
    {
        return {x - vector.x, y - vector.y};
    }

    Vector2 Vector2::operator-() const
    {
        return {-x, -y};
    }

    Vector2 operator*(const float scalar, const Vector2& vector)
    {
        return {vector.x * scalar, vector.y * scalar};
    }

    Vector2 operator*(const Vector2& vector, const float scalar)
    {
        return {vector.x * scalar, vector.y * scalar};
    }

    Vector2 operator/(const Vector2& vector, const float scalar)
    {
        return { vector.x / scalar, vector.y / scalar };
    }

    Vector2& Vector2::operator*=(const float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& Vector2::operator*=(const Vector2& scalar)
    {
        x *= scalar.x;
        y *= scalar.y;
        return *this;
    }

    Vector2& Vector2::operator/=(const float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    Vector2& Vector2::operator/=(const Vector2& vector)
    {
        x /= vector.x;
        y /= vector.y;
        return *this;
    }

    Vector2& Vector2::operator+=(const Vector2& vector)
    {
        x *= vector.x;
        y *= vector.y;
        return *this;
    }

    Vector2& Vector2::operator-=(const Vector2& vector)
    {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }

    Vector2 Vector2::Apply(float(* function)(float)) const
    {
        NOVA_ASSERT(function, "Supplied a nullptr function!");
        float x = function(this->x);
        float y = function(this->y);
        return { x, y };
    }

    bool Vector2::operator==(const Vector2& vector) const
    {
        return Math::AreSame(x, vector.x)
            && Math::AreSame(y, vector.y);
    }

    Vector2 Vector2::operator*(const Vector2& vector) const
    {
        return {x * vector.x, y * vector.y};
    }

    float& Vector2::operator[](const size_t index)
    {
        return ValuePtr()[index];
    }

    const float& Vector2::operator[](const size_t index) const
    {
        return ValuePtr()[index];
    }
}
