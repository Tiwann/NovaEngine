#pragma once


namespace Nova
{
    struct Vector3;
    
    struct Vector2
    {
        union{ float x = 0.0f, r; };
        union{ float y = 0.0f, g; };

        constexpr Vector2() = default;
        Vector2(float x, float y);
        explicit Vector2(float value);
        explicit Vector2(const Vector3& vector);

        float Magnitude() const;
        float* ValuePtr();
        const float* ValuePtr() const;
        float Dot(const Vector2& vector) const;

        Vector2 WithX(float x) const;
        Vector2 WithY(float y) const;
        Vector2 Normalized() const;
        Vector2 Apply(float (*function)(float)) const;

        Vector2 operator+(const Vector2& vector) const;
        Vector2 operator-(const Vector2& vector) const;
        Vector2 operator-() const;
        Vector2& operator+=(const Vector2& vector);
        Vector2& operator-=(const Vector2& vector);

        friend Vector2 operator*(float scalar, const Vector2& vector);
        friend Vector2 operator*(const Vector2& vector, float scalar);
        friend Vector2 operator/(const Vector2& vector, float scalar);

        Vector2& operator*=(float scalar);
        Vector2& operator*=(const Vector2& scalar);

        Vector2& operator/=(float scalar);
        Vector2& operator/=(const Vector2& vector);

        bool operator==(const Vector2& vector) const;
        Vector2 operator*(const Vector2& vector) const;

        float& operator[](size_t index);
        const float& operator[](size_t index) const;

        static float Dot(const Vector2& vectorA, const Vector2& vectorB);
        static Vector2 Normalize(const Vector2& vector);
        static float Angle(const Vector2& vectorA, const Vector2& vectorB);
        static Vector2 Lerp(const Vector2& A, const Vector2& B, float Alpha);
        static Vector2 QuadraticBezier(const Vector2& A, const Vector2& B, const Vector2& C, float Alpha);
        static Vector2 CubicBezier(const Vector2& A, const Vector2& B, const Vector2& C, const Vector2& D, float Alpha);

        static const Vector2 Zero;
        static const Vector2 One;
        static const Vector2 Right;
        static const Vector2 Up;
        static const Vector2 Left;
        static const Vector2 Down;
    };
}
