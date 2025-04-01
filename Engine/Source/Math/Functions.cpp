#include "Functions.h"
#include "Vector2.h"
#include "Vector3.h"


namespace Nova
{
    f32 Math::Cos(f32 Val)
    {
        return cosf(Val);
    }

    f32 Math::Sin(f32 Val)
    {
        return sinf(Val);
    }

    f32 Math::Tan(f32 Val)
    {
        return tanf(Val);
    }

    f32 Math::Acos(f32 Val)
    {
        return acosf(Val);
    }

    f32 Math::Asin(f32 Val)
    {
        return asinf(Val);
    }

    f32 Math::Atan(f32 Val)
    {
        return atanf(Val);
    }

    f32 Math::Abs(f32 Value)
    {
        return Value > 0 ? Value : -Value;
    }

    u32 Math::Fact(u32 Value)
    {
        if(Value == 0) return 1;
        return Value * Fact(Value - 1);
    }
    

    f32 Math::Lerp(f32 A, f32 B, f32 Alpha)
    {
        return A + Alpha * (B - A);
    }

    f32 Math::Map(f32 Value, f32 MinA, f32 MaxA, f32 MinB, f32 MaxB)
    {
        return MinB + (MaxB - MinB) * ((Value - MinA) / (MaxA - MinA));
    }

    f32 Math::Floor(f32 Value)
    {
        return floorf(Value);
    }

    f32 Math::Ceil(f32 Value)
    {
        return ceilf(Value);
    }

    f32 Math::Log2(f32 Value)
    {
        return log2f(Value);
    }

    f32 Math::Log10(f32 Value)
    {
        return log10f(Value);
    }

    f32 Math::Exp(f32 Value)
    {
        return Pow(E, Value);
    }

    f32 Math::Smoothstep(f32 Value, f32 Min, f32 Max)
    {
        const f32 Val = Clamp((Value - Min) / (Max - Min), 0.0f, 1.0f);
        return Val * Val * (3.0f - 2.0f * Val);
    }

    f32 Math::MappedSin(f32 Val, f32 Min, f32 Max)
    {
        return Map(Sin(Val), -1.0f, 1.0f, Min, Max);
    }

    f32 Math::MappedCos(f32 Val, f32 Min, f32 Max)
    {
        return Map(Cos(Val), -1.0f, 1.0f, Min, Max);
    }

    f32 Math::Sign(f32 Val)
    {
        return Val > 0.0f ? 1.0f : Val < 0.0f ? -1.0f : 0.0f;
    }

    f32 Math::Distance(const Vector2& VecA, const Vector2& VecB)
    {
        return Sqrt(Pow(VecB.x - VecA.x, 2) + Pow(VecB.y - VecA.y, 2));
    }

    f32 Math::Distance(const Vector3& VecA, const Vector3& VecB)
    {
        return Sqrt(Pow(VecB.x - VecA.x, 2) + Pow(VecB.y - VecA.y, 2) + Pow(VecB.z - VecA.z, 2));
    }
    
    
    bool Math::AreSame(f32 Lhs, f32 Rhs)
    {
        return Abs(Lhs - Rhs) <= std::numeric_limits<f32>::epsilon();
    }

    bool Math::AreDifferent(f32 Lhs, f32 Rhs)
    {
        return !AreSame(Lhs, Rhs);
    }

    bool Math::IsZero(f32 Val)
    {
        return AreSame(Val, 0.0f);
    }

    f32 Math::Sqrt(f32 Val)
    {
        return sqrtf(Val);
    }

    f32 Math::Pow(f32 Val, f32 Exp)
    {
        return powf(Val, Exp);
    }

    f32 Math::Radians(f32 Degrees)
    {
        return Degrees * Pi / 180.0f;
    }

    f32 Math::Degrees(f32 Radians)
    {
        return Radians * 180.0f / Pi;
    }

    f32 Math::MoveTowards(f32 Current, f32 Target, f32 MaxDelta)
    {
        const f32 MovedValue = Current + Sign(Target - Current) * MaxDelta;
        return Abs(Target - Current) <= MaxDelta ? Target : MovedValue;
    }

    f32 Math::SmoothDamp(f32 Current, f32 Target, f32& CurrentVelocity, f32 SmoothTime, f32 Delta, f32 MaxSpeed)
    {
        SmoothTime = Max(0.0001f, SmoothTime);
        const f32 Omega = 2.0f / SmoothTime;

        const f32 X = Omega * Delta;
        const f32 Exp = 1.0f / (1.0f + X + 0.48F * X * X + 0.235F * X * X * X);
        f32 Change = Current - Target;
        const f32 OriginalTo = Target;

        // Clamp maximum speed
        const f32 MaxChange = MaxSpeed * SmoothTime;
        Change = Clamp(Change, -MaxChange, MaxChange);
        Target = Current - Change;

        const f32 Temp = (CurrentVelocity + Omega * Change) * Delta;
        CurrentVelocity = (CurrentVelocity - Omega * Temp) * Exp;
        f32 Output = Target + (Change + Temp) * Exp;

        // Prevent overshooting
        if (OriginalTo - Current > 0.0F == Output > OriginalTo)
        {
            Output = OriginalTo;
            CurrentVelocity = (Output - OriginalTo) / Delta;
        }

        return Output;
    }

    i32 Math::IntegerPart(f32 Val)
    {
        return (i32)Floor(Val);
    }

    f32 Math::DecimalPart(f32 Val)
    {
        return Val - (f32)IntegerPart(Val);
    }

    bool Math::IsBetween(f32 Val, f32 Min, f32 Max)
    {
        return Val >= Min && Val <= Max;
    }

    f32 Math::Wrap(f32 Value, f32 Min, f32 Max)
    {
        return Value < Min ? Max - Abs(Min - Value) : Value > Max ? Min + Abs(Value - Max) : Value;
    }

    Vector2 Math::Wrap(const Vector2& Value, f32 Min, f32 Max)
    {
        return {Wrap(Value.x, Min, Max), Wrap(Value.y, Min, Max)};
    }

    Vector3 Math::Wrap(const Vector3& Value, f32 Min, f32 Max)
    {
        return {Wrap(Value.x, Min, Max), Wrap(Value.y, Min, Max), Wrap(Value.z, Min, Max)};
    }
}
