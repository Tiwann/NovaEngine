#pragma once
#include "Vector3.h"


namespace Nova
{
    struct Vector2;
    class Matrix4;

    class Matrix3
    {
    public:
        Matrix3();
        Matrix3(const Vector3& Col1, const Vector3& Col2, const Vector3& Col3);
        Matrix3(const Matrix4& Matrix);
        
        const float* ValuePtr() const;
        float Magnitude() const;
        float Determinant() const;
        Matrix3 Inverted() const;
        Matrix3 Transposed() const;

        bool operator==(const Matrix3& Other) const;
        Vector3 operator*(const Vector3& Vec) const;
        Matrix3 operator*(const Matrix3& Mat) const;
        Matrix3& operator*(float Scalar);

        Vector3& operator[](size_t i);
        const Vector3& operator[](size_t i) const;
        Vector3 GetRow(size_t i) const;

        void Translate(const Vector2& Vec);
        void Rotate(const Vector3& Axis, float Radians);
        void RotateDegrees(const Vector3& Axis, float Degrees);
        void Scale(float Scalar);
        void Scale(const Vector3& Scale);
        
        static Matrix3 Identity;
        static Matrix3 One;
    private:
        union
        {
            Vector3 Columns[3];
            struct { float m00, m10, m20, m01, m11, m21, m02, m12, m22; };
        };
    };
}
