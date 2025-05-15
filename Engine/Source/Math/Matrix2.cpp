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

    Matrix2 Matrix2::Inverted() const
    {
        const f32 OneOverDeterminant = 1.0f / Determinant();

        const Matrix2 Inverse
        {
            Vector2(Columns[1].y * OneOverDeterminant, -Columns[0].y * OneOverDeterminant),
            Vector2(-Columns[1].x * OneOverDeterminant, Columns[0].x * OneOverDeterminant),
        };
        return Inverse;
    }

    Matrix2 Matrix2::Transposed() const
    {
        Matrix2 Result = *this;
        std::swap(Result[0].y, Result[1].x);
        return Result;
    }

    Vector2 Matrix2::operator*(const Vector2& Vec) const
    {
        const Vector2 Row0 = GetRow(0);
        const Vector2 Row1 = GetRow(1);
        const Vector2 Result = { Row0.Dot(Vec), Row1.Dot(Vec) };
        return Result;
    }

    Matrix2 Matrix2::operator*(const Matrix2& Mat) const
    {
        const Vector2 Row0 = GetRow(0);
        const Vector2 Row1 = GetRow(1);

        const Vector2& Col0 = Mat.Columns[0];
        const Vector2& Col1 = Mat.Columns[1];

        const Vector2 NewCol0 { Row0.Dot(Col0), Row1.Dot(Col0) };
        const Vector2 NewCol1 { Row0.Dot(Col1), Row1.Dot(Col1) };
        return Matrix2 { NewCol0, NewCol1 };
    }

    Vector2& Matrix2::operator[](const size_t i)
    {
        NOVA_ASSERT(i < 2, "Cannot access Mat2 element: index out of bounds.");
        return Columns[i];
    }

    const Vector2& Matrix2::operator[](const size_t i) const
    {
        NOVA_ASSERT(i < 2, "Cannot access Mat2 element: index out of bounds.");
        return Columns[i];
    }

    Vector2 Matrix2::GetRow(const size_t i) const
    {
        NOVA_ASSERT(i < 2, "Cannot access Mat2 element: index out of bounds.");
        switch (i)
        {
            case 0: return { Columns[0].x, Columns[1].x };
            case 1: return { Columns[0].y, Columns[1].y };
            default: throw;
        }
    }

    Matrix2& Matrix2::operator*(const f32 Scalar)
    {
        Columns[0] *= Scalar;
        Columns[1] *= Scalar;
        return *this;
    }

    void Matrix2::Rotate(const f32 Radians)
    {
        *this = Math::Rotate(*this, Radians);
    }

    void Matrix2::RotateDegrees(const f32 Degrees)
    {
        *this = Math::RotateDegrees(*this, Degrees);
    }

    void Matrix2::Scale(const f32 Scalar)
    {
        *this = Math::Scale(*this, Scalar);
    }

    void Matrix2::Scale(const Vector2& Scale)
    {
        *this = Math::Scale(*this, Scale);
    }
}
