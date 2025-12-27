#pragma once
#include "Vector4.h"


namespace Nova
{
    struct Vector2;
    struct Vector3;
    struct Quaternion;
    struct Matrix2;
    
    class Matrix4
    {
    public:
        Matrix4();
        Matrix4(const Vector4& Col1, const Vector4& Col2, const Vector4& Col3, const Vector4& Col4);

        const float* ValuePtr() const;

        float Magnitude() const;
        float Determinant() const;
        Matrix4 Inverted() const;
        Matrix4 Transposed() const;

        Vector4 operator*(const Vector4& Vec) const;
        Vector3 operator*(const Vector3& Vec) const;
        Vector2 operator*(const Vector2& Vec) const;
        Matrix4 operator*(const Matrix4& Mat) const;
        Matrix4 operator*(float Scalar) const;

        Vector4& operator[](size_t i);
        const Vector4& operator[](size_t i) const;
        Vector4 GetRow(size_t i) const;


        void Rotate(const Vector3& Axis, float Radians);
        void Rotate(const Vector3& EulerAngles);
        void Rotate(const Quaternion& Rotation);
        void RotateDegrees(const Vector3& Axis, float Degrees);
        void RotateDegrees(const Vector3& EulerAnglesDegrees);
        void Scale(float Scalar);
        void Scale(const Vector3& Scale);
        void Translate(const Vector3& Translation);
        void Decompose(Vector3* position, Quaternion* rotation, Vector3* scale) const;

        static const Matrix4 Identity;
        static const Matrix4 One;
        static Matrix4 TRS(const Vector3& Position, const Vector3& EulerAnglesDegrees, const Vector3& Scale);
        static Matrix4 TRS(const Vector3& Position, const Quaternion& Orientation, const Vector3& Scale);
    private:
        union
        {
            Vector4 columns[4];
            struct { float m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m32, m03, m13, m23, m33; };
        };
    };
}
