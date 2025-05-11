#pragma once
#include "Vector3.h"
#include <CoreExport.h>

namespace Nova
{
    struct Vector2;

    class CORE_API Matrix3
    {
    public:
        Matrix3();
        Matrix3(const Vector3& Col1, const Vector3& Col2, const Vector3& Col3);
        
        const f32* ValuePtr() const;
        f32 Magnitude() const;
        f32 Determinant() const;
        Matrix3 Inverted() const;

        Vector3 operator*(const Vector3& Vec) const;
        Matrix3 operator*(const Matrix3& Mat) const;
        Matrix3& operator*(f32 Scalar);

        Vector3& operator[](size_t i);
        const Vector3& operator[](size_t i) const;
        Vector3 GetRow(size_t i) const;

        void Translate(const Vector2& Vec);
        void Rotate(const Vector3& Axis, f32 Radians);
        void RotateDegrees(const Vector3& Axis, f32 Degrees);
        void Scale(f32 Scalar);
        void Scale(const Vector3& Scale);
        
        static Matrix3 Identity;
        static Matrix3 One;
    private:
        union
        {
            Vector3 Columns[3];
            struct { f32 m00, m10, m20, m01, m11, m21, m02, m12, m22; };
        };
    };
}
