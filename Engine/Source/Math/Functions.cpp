#include "Functions.h"
#include "Vector2.h"
#include "Vector3.h"


namespace Nova
{
    float Math::Cos(const float value)
    {
        return cosf(value);
    }

    float Math::Sin(const float value)
    {
        return sinf(value);
    }

    float Math::Tan(const float value)
    {
        return tanf(value);
    }

    float Math::Acos(const float value)
    {
        return acosf(value);
    }

    float Math::Asin(const float value)
    {
        return asinf(value);
    }

    float Math::Atan(const float value)
    {
        return atanf(value);
    }

    float Math::Atan2(const float y, const float x)
    {
        return atan2f(y, x);
    }

    float Math::Abs(const float value)
    {
        return value > 0 ? value : -value;
    }

    uint32_t Math::Fact(const uint32_t value)
    {
        if(value == 0) return 1;
        return value * Fact(value - 1);
    }


    float Math::Saturate(const float value)
    {
        return Clamp(value, 0.0f, 1.0f);
    }

    float Math::Lerp(const float a, const float b, const float alpha)
    {
        return a + alpha * (b - a);
    }

    float Math::Map(const float value, const float minA, const float maxA, const float minB, const float maxB)
    {
        return minB + (maxB - minB) * ((value - minA) / (maxA - minA));
    }

    float Math::Floor(const float value)
    {
        return floorf(value);
    }

    float Math::Ceil(const float value)
    {
        return ceilf(value);
    }

    float Math::Log2(const float value)
    {
        return log2f(value);
    }

    float Math::Log10(const float value)
    {
        return log10f(value);
    }

    float Math::Exp(const float value)
    {
        return Pow(E, value);
    }

    float Math::Smoothstep(const float value, const float min, const float max)
    {
        const float Val = Clamp((value - min) / (max - min), 0.0f, 1.0f);
        return Val * Val * (3.0f - 2.0f * Val);
    }

    float Math::MappedSin(const float value, const float min, const float max)
    {
        return Map(Sin(value), -1.0f, 1.0f, min, max);
    }

    float Math::MappedCos(const float value, const float min, const float max)
    {
        return Map(Cos(value), -1.0f, 1.0f, min, max);
    }

    float Math::Sign(const float value)
    {
        return value > 0.0f ? 1.0f : value < 0.0f ? -1.0f : 0.0f;
    }

    float Math::Distance(const Vector2& a, const Vector2& b)
    {
        return Sqrt(Pow(b.x - a.x, 2) + Pow(b.y - a.y, 2));
    }

    float Math::Distance(const Vector3& a, const Vector3& b)
    {
        return Sqrt(Pow(b.x - a.x, 2) + Pow(b.y - a.y, 2) + Pow(b.z - a.z, 2));
    }


    bool Math::AreSame(const float lhs, const float rhs)
    {
        return Abs(lhs - rhs) <= Epsilon;
    }

    bool Math::AreDifferent(const float lhs, const float rhs)
    {
        return !AreSame(lhs, rhs);
    }

    bool Math::IsZero(const float value)
    {
        return AreSame(value, 0.0f);
    }

    float Math::Sqrt(const float value)
    {
        return sqrtf(value);
    }

    float Math::Pow(const float value, const float exp)
    {
        return powf(value, exp);
    }

    float Math::Radians(const float degrees)
    {
        return degrees * Pi / 180.0f;
    }

    float Math::Degrees(const float radians)
    {
        return radians * 180.0f / Pi;
    }

    float Math::MoveTowards(const float current, const float target, const float maxDelta)
    {
        const float MovedValue = current + Sign(target - current) * maxDelta;
        return Abs(target - current) <= maxDelta ? target : MovedValue;
    }

    float Math::SmoothDamp(const float current, float target, float& currentVelocity, float smoothTime, const float delta, const float maxSpeed)
    {
        smoothTime = Max(0.0001f, smoothTime);
        const float omega = 2.0f / smoothTime;

        const float x = omega * delta;
        const float exp = 1.0f / (1.0f + x + 0.48F * x * x + 0.235F * x * x * x);
        float change = current - target;
        const float originalTo = target;

        // Clamp maximum speed
        const float maxChange = maxSpeed * smoothTime;
        change = Clamp(change, -maxChange, maxChange);
        target = current - change;

        const float temporary = (currentVelocity + omega * change) * delta;
        currentVelocity = (currentVelocity - omega * temporary) * exp;
        float output = target + (change + temporary) * exp;

        // Prevent overshooting
        if (originalTo - current > 0.0F == output > originalTo)
        {
            output = originalTo;
            currentVelocity = (output - originalTo) / delta;
        }

        return output;
    }

    int32_t Math::IntegerPart(const float value)
    {
        return (int32_t)Floor(value);
    }

    float Math::DecimalPart(const float value)
    {
        return value - (float)IntegerPart(value);
    }

    bool Math::IsBetween(const float value, const float min, const float max)
    {
        return value >= min && value < max;
    }

    float Math::Wrap(const float value, const float min, const float max)
    {
        return value < min ? max - Abs(min - value) : value > max ? min + Abs(value - max) : value;
    }

    Vector2 Math::Wrap(const Vector2& value, const float min, const float max)
    {
        return {Wrap(value.x, min, max), Wrap(value.y, min, max)};
    }

    Vector3 Math::Wrap(const Vector3& value, const float min, const float max)
    {
        return {Wrap(value.x, min, max), Wrap(value.y, min, max), Wrap(value.z, min, max)};
    }
}
