#include "Matrix2.h"
#include "Functions.h"
#include "Runtime/Assertion.h"

namespace Nova
{
    Matrix2 Matrix2::Identity = Matrix2();
    Matrix2 Matrix2::One = { Vector2::One, Vector2::One };
    
    Matrix2::Matrix2(): m00(1.0f), m10(0.0f), m01(0.0f), m11(1.0f)
    {

    }

    Matrix2::Matrix2(const Vector2& Col1, const Vector2& Col2)
    {
        Columns[0] = Col1;
        Columns[1] = Col2;
    }

    const f32* Matrix2::ValuePtr() const
    {
        return (const f32*)this;
    }

    f32 Matrix2::Magnitude() const
    {
        return Math::Sqrt(m00*m00 + m01*m01 + m10*m10 + m11*m11);
    }

    f32 Matrix2::Determinant() const
    {
        return m00 * m11 - m01 * m10;
    }

    Vector2 Matrix2::Multiply(const Vector2& Vec) const
    {
        return Columns[0] * Vec.x + Columns[1] * Vec.y;
    }

    Vector2 Matrix2::operator*(const Vector2& Vec) const
    {
        return Multiply(Vec);
    }

    Matrix2 Matrix2::Multiply(const Matrix2& Mat) const
    {
        const Vector2 Col1 { Mat.m00 * m00 + Mat.m10 * m01, Mat.m00 * m10 + Mat.m10 * m11 };
        const Vector2 Col2 { Mat.m01 * m00 + Mat.m11 * m01, Mat.m01 * m10 + Mat.m11 * m11 };
        return Matrix2 { Col1, Col2 };
    }

    Matrix2 Matrix2::operator*(const Matrix2& Mat) const
    {
        return Multiply(Mat);
    }

    Vector2& Matrix2::operator[](size_t i)
    {
        NOVA_ASSERT(i < 2, "Cannot access Mat2 element: index out of bounds.");
        return Columns[i];
    }

    const Vector2& Matrix2::operator[](size_t i) const
    {
        NOVA_ASSERT(i < 2, "Cannot access Mat2 element: index out of bounds.");
        return Columns[i];
    }

    Matrix2& Matrix2::operator*(f32 Scalar)
    {
        Columns[0] *= Scalar;
        Columns[1] *= Scalar;
        return *this;
    }

    void Matrix2::Rotate(f32 Radians)
    {
        *this = Math::Rotate(*this, Radians);
    }

    void Matrix2::RotateDegrees(f32 Degrees)
    {
        *this = Math::RotateDegrees(*this, Degrees);
    }

    void Matrix2::Scale(f32 Scalar)
    {
        *this = Math::Scale(*this, Scalar);
    }

    void Matrix2::Scale(const Vector2& Scale)
    {
        *this = Math::Scale(*this, Scale);
    }
}
