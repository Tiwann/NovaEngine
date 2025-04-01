#pragma once
#include "Runtime/Types.h"
#include "Runtime/TypeTraits.h"
#include "Runtime/Color.h"
#include <cmath>
#include <limits>
#include <numbers>
#include <CoreExport.h>


namespace Nova
{
    struct CORE_API Math
    {
        Math() = delete;

        static constexpr f32 Pi = std::numbers::pi_v<f32>;
        static constexpr f32 Tau = 2.0f * Pi;
        static constexpr f32 PiHalf = Pi / 2.0f;
        static constexpr f32 E = std::numbers::e_v<f32>;
        static constexpr f32 Infinity = std::numeric_limits<f32>::infinity();
        
        static f32 Cos(f32 Val);
        static f32 Sin(f32 Val);
        static f32 Tan(f32 Val);
        static f32 Acos(f32 Val);
        static f32 Asin(f32 Val);
        static f32 Atan(f32 Val);
        static f32 Abs(f32 Value);
        static u32 Fact(u32 Value);
        template<typename F> requires IsFloatingPointValue<F>
        static F Clamp(F Value, F Min, F Max)
        {
            return Value < Min ? Min : Value > Max ? Max : Value;
        }
        static f32 Lerp(f32 A, f32 B, f32 Alpha);
        static f32 Map(f32 Value, f32 MinA, f32 MaxA, f32 MinB, f32 MaxB);
        static f32 Floor(f32 Value);
        static f32 Ceil(f32 Value);
        static f32 Log2(f32 Value);
        static f32 Log10(f32 Value);
        static f32 Exp(f32 Value);
        static f32 Smoothstep(f32 Value, f32 Min, f32 Max);
        static f32 MappedSin(f32 Val, f32 Min, f32 Max);
        static f32 MappedCos(f32 Val, f32 Min, f32 Max);

        template<typename T, typename U> requires(IsIntegerValue<T> && IsArithmeticValue<U>)
        static T NearestPowerOfTwo(U Number)
        {
            if(Number < 1) return 1;

            T Lower = (T)Pow(2, Floor(log2f((f32)Number)));
            T Upper = (T)Pow(2, Ceil(log2f((f32)Number)));

            return Number - Lower < Upper - Number ? Lower : Upper;
        }


        template<typename T> requires IsArithmeticValue<T>
        static constexpr T Max(T A, T B)
        {
            return A > B ? A : B;
        }

        template<typename T> requires IsArithmeticValue<T>
        static constexpr T Min(T A, T B)
        {
            return A < B ? A : B;
        }
        
        static f32 Sign(f32 Val);
        static bool AreSame(f32 Lhs, f32 Rhs);
        static bool AreDifferent(f32 Lhs, f32 Rhs);
        static bool IsZero(f32 Val);
        static f32 Sqrt(f32 Val);
        static f32 Pow(f32 Val, f32 Exp);
        static f32 Radians(f32 Degrees);
        static f32 Degrees(f32 Radians);
        static f32 MoveTowards(f32 Current, f32 Target, f32 MaxDelta);
        static f32 SmoothDamp(f32 Current, f32 Target, f32& CurrentVelocity, f32 SmoothTime,  f32 Delta, f32 MaxSpeed = Infinity);
        static i32 IntegerPart(f32 Val);
        static f32 DecimalPart(f32 Val);
        static bool IsBetween(f32 Val, f32 Min, f32 Max);
        static f32 Wrap(f32 Value, f32 Min, f32 Max);
        static Vector2 Wrap(const Vector2& Value, f32 Min, f32 Max);
        static Vector3 Wrap(const Vector3& Value, f32 Min, f32 Max);


        template<typename VectorType> requires IsVectorValue<VectorType>
        static f32 Dot(const VectorType& A, const VectorType& B)
        {
            return A.Dot(B);
        }

        template<typename VectorType> requires IsVectorValue<VectorType>
        static VectorType Normalize(const VectorType& Value)
        {
            return Value.Normalized();
        }

        template<typename T> requires IsVectorValue<T> || IsMatrixValue<T>
        static f32* ValuePointer(const T& Value)
        {
            return Value.ValuePtr();
        }

        template<typename T> requires IsVectorValue<T> || IsMatrixValue<T>
        static const f32* ValuePointer(const T& Value)
        {
            return Value.ValuePtr();
        }

        template<typename T> requires IsVectorValue<T> || IsFloatingPointValue<T> || IsColorValue<T>
        static T Lerp(const T& A, const T& B, f32 Alpha)
        {
            if constexpr (IsFloatingPointValue<T>) return Math::Lerp(A, B, Alpha);
            if constexpr (IsVectorValue<T>) return T::Lerp(A, B, Alpha);
            if constexpr(IsColorValue<T>) return Color::Lerp(A, B, Alpha);
            return T();
        }
        
        static class Matrix2 Rotate(const Matrix2& Mat, f32 Radians);
        static Matrix2 RotateDegrees(const Matrix2& Mat, f32 Degrees);
        static Matrix2 Scale(const Matrix2& Mat, const struct Vector2& Scale);
        static Matrix2 Scale(const Matrix2& Mat, f32 Scale);

        static class Matrix3 Rotate(const Matrix3& Mat, const struct Vector3& Axis, f32 Radians);
        static Matrix3 RotateDegrees(const Matrix3& Mat, const Vector3& Axis, f32 Degrees);
        static Matrix3 Scale(const Matrix3& Mat, const Vector3& Scale);
        static Matrix3 Scale(const Matrix3& Mat, f32 Scale);
        
        static class Matrix4 RotateAxisAngle(const Matrix4& Mat, const Vector3& Axis, f32 AngleRadians); 
        static Matrix4 RotateAxisAngleDegrees(const Matrix4& Mat, const Vector3& Axis, f32 AngleDegrees);
        static Matrix4 Scale(const Matrix4& Mat, const Vector3& Scale);
        static Matrix4 Scale(const Matrix4& Mat, f32 Scale);
        static Matrix4 RotateEulerAngles(const Matrix4& Mat, const Vector3& EulerAnglesRadians);
        static Matrix4 RotateEulerAnglesDegrees(const Matrix4& Mat, const Vector3& EulerAnglesDegrees);
        static Matrix4 RotateEulerAnglesAroundPoint(const Matrix4& Mat, const Vector3& EulerAnglesRadians, const Vector3& Point);
        static Matrix4 RotateEulerAnglesAroundPointDegrees(const Matrix4& Mat, const Vector3& EulerAnglesDegrees, const Vector3& Point);
        static Matrix4 RotateAxisAngleAroundPoint(const Matrix4& Mat, const Vector3& Axis, f32 AngleRadians, const Vector3& Point);
        static Matrix4 RotateAxisAngleAroundPointDegrees(const Matrix4& Mat, const Vector3& Axis, f32 AngleDegrees, const Vector3& Point);
        static Matrix4 LookAt(const Vector3& Eye, const Vector3& Center, const Vector3& Up);
        static Matrix4 LocalToWorldNormal(const Vector3& Translation, const Vector3& EulerAnglesDegrees);

        static f32 Distance(const Vector2& VecA, const Vector2& VecB);
        static f32 Distance(const Vector3& VecA, const Vector3& VecB);
        
        static Matrix4 Perspective(f32 FOV, f32 AspectRatio, f32 Near, f32 Far);
        static Matrix4 Orthographic(f32 Width, f32 Height, f32 Scale, f32 Near, f32 Far);
        static Matrix4 Translate(const Matrix4& Mat, const Vector3& Translation);


        static Vector3 ForwardFromRotation(const Vector3& EulerAngles);
        static Vector3 UpFromRotation(const Vector3& EulerAngles);
        static Vector3 RightFromRotation(const Vector3& EulerAngles);
    };
}
