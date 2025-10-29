#pragma once


namespace Nova
{
    struct Vector3;
    
    struct Vector2
    {
        union{ float x = 0.0f, r; };
        union{ float y = 0.0f, g; };

        constexpr Vector2() = default;
        Vector2(float X, float Y);
        explicit Vector2(float Value);
        explicit Vector2(const Vector3& Vec);

        float Magnitude() const;
        float* ValuePtr();
        const float* ValuePtr() const;
        float Dot(const Vector2& Vec) const;

        Vector2 WithX(float X) const;
        Vector2 WithY(float Y) const;
        Vector2 Normalized() const;
        Vector2 Apply(float (*Function)(float)) const;

        Vector2 operator+(const Vector2& Vec) const;
        Vector2 operator-(const Vector2& Vec) const;
        Vector2 operator-() const;
        Vector2& operator+=(const Vector2& Vec);
        Vector2& operator-=(const Vector2& Vec);

        friend Vector2 operator*(float Scalar, const Vector2& Vec);
        friend Vector2 operator*(const Vector2& Vec, float Scalar);
        friend Vector2 operator/(const Vector2& Vec, float Scalar);

        Vector2& operator*=(float Scalar);
        Vector2& operator*=(const Vector2& Scalar);

        Vector2& operator/=(float Scalar);
        Vector2& operator/=(const Vector2& Vec);

        bool operator==(const Vector2& Vec) const;
        Vector2 operator*(const Vector2& Vec) const;

        float& operator[](size_t Index);
        const float& operator[](size_t Index) const;

        static float Dot(const Vector2& VecA, const Vector2& VecB);
        static Vector2 Normalize(const Vector2& Vec);
        static float Angle(const Vector2& VecA, const Vector2& VecB);
        static Vector2 Lerp(const Vector2& A, const Vector2& B, float Alpha);
        static Vector2 QuadraticBezier(const Vector2& A, const Vector2& B, const Vector2& C, float Alpha);
        static Vector2 CubicBezier(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, float Alpha);

        static Vector2 Zero;
        static Vector2 One;
        static Vector2 Right;
        static Vector2 Up;
        static Vector2 Left;
        static Vector2 Down;
    };
}
