#pragma once
#include "Vector2.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API Matrix2
    {
    public:
        Matrix2();
        Matrix2(const Vector2& Col1, const Vector2& Col2);
        
        const f32* ValuePtr() const;
        f32 Magnitude() const;
        f32 Determinant() const;
        Matrix2 Inverted() const;

        Vector2 operator*(const Vector2& Vec) const;
        Matrix2 operator*(const Matrix2& Mat) const;
        Matrix2& operator*(f32 Scalar);

        Vector2& operator[](size_t i);
        const Vector2& operator[](size_t i) const;

        Vector2 GetRow(size_t i) const;

        void Rotate(f32 Radians);
        void RotateDegrees(f32 Degrees);
        void Scale(f32 Scalar);
        void Scale(const Vector2& Scale);
        
        static Matrix2 Identity;
        static Matrix2 One;
    private:
        union
        {
            Vector2 Columns[2]{ Vector2(0.0f), Vector2(0.0f) };
            struct { f32 m00, m10, m01, m11; };
        };
    };
}
