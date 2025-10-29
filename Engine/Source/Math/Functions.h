#pragma once
#include "Runtime/TypeTraits.h"
#include "Runtime/Color.h"

#include <cmath>
#include <limits>
#include <numbers>


namespace Nova
{
    struct Math
    {
        Math() = delete;

        static constexpr float Pi = std::numbers::pi_v<float>;
        static constexpr float Tau = 2.0f * Pi;
        static constexpr float PiHalf = Pi / 2.0f;
        static constexpr float E = std::numbers::e_v<float>;
        static constexpr float Infinity = std::numeric_limits<float>::infinity();
        static constexpr float Epsilon = std::numeric_limits<float>::epsilon();
        
        static float Cos(float value);
        static float Sin(float value);
        static float Tan(float value);
        static float Acos(float value);
        static float Asin(float value);
        static float Atan(float value);
        static float Atan2(float y, float x);
        static float Abs(float value);
        static uint32_t Fact(uint32_t value);
        template<typename F> requires IsFloatingPointValue<F>
        static F Clamp(const F& value, const F& min, const F& max)
        {
            return value < min ? min : value > max ? max : value;
        }

        static float Saturate(float value);
        static float Lerp(float a, float b, float alpha);
        static float Map(float value, float minA, float maxA, float minB, float maxB);
        static float Floor(float value);
        static float Ceil(float value);
        static float Log2(float value);
        static float Log10(float value);
        static float Exp(float value);
        static float Smoothstep(float value, float min, float max);
        static float MappedSin(float value, float min, float max);
        static float MappedCos(float value, float min, float max);

        template<typename T, typename U> requires(IsIntegerValue<T> && IsArithmeticValue<U>)
        static T NearestPowerOfTwo(U number)
        {
            if (number == 0)
                return 1;

            --number;
            number |= number >> 1;
            number |= number >> 2;
            number |= number >> 4;
            number |= number >> 8;
            number |= number >> 16;
            return number + 1;
        }


        template<typename T> requires IsArithmeticValue<T>
        static constexpr T Max(T lhs, T rhs)
        {
            return lhs > rhs ? lhs : rhs;
        }

        template<typename T> requires IsArithmeticValue<T>
        static constexpr T Min(T lhs, T rhs)
        {
            return lhs < rhs ? lhs : rhs;
        }
        
        static float Sign(float value);
        static bool AreSame(float lhs, float rhs);
        static bool AreDifferent(float lhs, float rhs);
        static bool IsZero(float value);
        static float Sqrt(float value);
        static float Pow(float value, float exp);
        static float Radians(float degrees);
        static float Degrees(float radians);
        static float MoveTowards(float current, float target, float maxDelta);
        static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime,  float delta, float maxSpeed = Infinity);
        static int32_t IntegerPart(float value);
        static float DecimalPart(float value);
        static bool IsBetween(float value, float min, float max);
        static float Wrap(float value, float min, float max);
        static Vector2 Wrap(const Vector2& value, float min, float max);
        static Vector3 Wrap(const Vector3& value, float min, float max);


        template<typename VectorType> requires IsVectorValue<VectorType>
        static float Dot(const VectorType& lhs, const VectorType& rhs)
        {
            return lhs.Dot(rhs);
        }

        template<typename VectorType> requires IsVectorValue<VectorType>
        static VectorType Normalize(const VectorType& value)
        {
            return value.Normalized();
        }

        template<typename T> requires IsVectorValue<T> || IsMatrixValue<T>
        static float* ValuePointer(const T& value)
        {
            return value.ValuePtr();
        }

        template<typename T> requires IsVectorValue<T> || IsMatrixValue<T>
        static const float* ValuePointer(const T& value)
        {
            return value.ValuePtr();
        }

        template<typename T> requires IsVectorValue<T> || IsFloatingPointValue<T> || IsColorValue<T>
        static T Lerp(const T& lhs, const T& rhs, float alpha)
        {
            if constexpr (IsFloatingPointValue<T>) return Math::Lerp(lhs, rhs, alpha);
            if constexpr (IsVectorValue<T>) return T::Lerp(lhs, rhs, alpha);
            if constexpr(IsColorValue<T>) return Color::Lerp(lhs, rhs, alpha);
            return T();
        }

        template<typename T> requires IsMatrixValue<T>
        static T Identity()
        {
            return T::Identity;
        }

        template<typename T> requires IsMatrixValue<T>
        static T Determinant(const T& mat)
        {
            return mat.Determinant();
        }


        template<typename T> requires IsMatrixValue<T> || IsQuaternionValue<T>
        static T Inverse(const T& in)
        {
            return in.Inverted();
        }

        template<typename T> requires IsMatrixValue<T>
        static T Transpose(const T& in)
        {
            return in.Transposed();
        }
        
        static class Matrix2 Rotate(const Matrix2& mat, float Radians);
        static Matrix2 RotateDegrees(const Matrix2& mat, float degrees);
        static Matrix2 Scale(const Matrix2& mat, const struct Vector2& scale);
        static Matrix2 Scale(const Matrix2& mat, float scale);

        static class Matrix3 RotateAxisAngle(const Matrix3& mat, const struct Vector3& axis, float angleRadians);
        static Matrix3 RotateAxisAngleDegrees(const Matrix3& mat, const Vector3& axis, float angleDegrees);
        static Matrix3 RotateEulerAngles(const Matrix3& mat, const Vector3& eulerAnglesRadians);
        static Matrix3 RotateEulerAnglesDegrees(const Matrix3& mat, const Vector3& eulerAnglesDegrees);
        static Matrix3 Scale(const Matrix3& mat, const Vector3& scale);
        static Matrix3 Scale(const Matrix3& mat, float scale);
        static Matrix3 Translate(const Matrix3& mat, const Vector2& translation);

        static class Matrix4 RotateAxisAngle(const Matrix4& mat, const Vector3& axis, float angleRadians);
        static Matrix4 RotateAxisAngleDegrees(const Matrix4& mat, const Vector3& axis, float angleDegrees);
        static Matrix4 Scale(const Matrix4& mat, const Vector3& scale);
        static Matrix4 Scale(const Matrix4& mat, float scale);
        static Matrix4 RotateEulerAngles(const Matrix4& mat, const Vector3& eulerAnglesRadians);
        static Matrix4 RotateEulerAnglesDegrees(const Matrix4& mat, const Vector3& eulerAnglesDegrees);
        static Matrix4 LocalToWorldNormal(const Vector3& translation, const Vector3& eulerAnglesDegrees);

        static float Distance(const Vector2& a, const Vector2& b);
        static float Distance(const Vector3& a, const Vector3& b);
        
        static Matrix4 Perspective(float fov, float aspectRatio, float near, float far);
        static Matrix4 Orthographic(float width, float height, float scale, float near, float far);
        static Matrix4 Translate(const Matrix4& mat, const Vector3& translation);


        static Vector3 ForwardFromRotation(const Quaternion& rotation);
        static Vector3 UpFromRotation(const Quaternion& rotation);
        static Vector3 RightFromRotation(const Quaternion& rotation);
    };
}
