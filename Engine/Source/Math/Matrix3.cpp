#include "Matrix3.h"
#include "Functions.h"
#include "Matrix2.h"
#include "Runtime/Assertion.h"

namespace Nova
{
    Matrix3 Matrix3::Identity = Matrix3();
    Matrix3 Matrix3::One = { Vector3::One, Vector3::One, Vector3::One };
    
    Matrix3::Matrix3()
    {
        Columns[0] = Vector3(1.0f, 0.0f, 0.0f);
        Columns[1] = Vector3(0.0f, 1.0f, 0.0f);
        Columns[2] = Vector3(0.0f, 0.0f, 1.0f);
    }

    Matrix3::Matrix3(const Vector3& Col1, const Vector3& Col2, const Vector3& Col3)
    {
        Columns[0] = Col1;
        Columns[1] = Col2;
        Columns[2] = Col3;
    }

    const f32* Matrix3::ValuePtr() const
    {
        return (const f32*)this;
    }

    f32 Matrix3::Magnitude() const
    {
        return Math::Sqrt(m00*m00 + m10*m10 + m20*m20 +
                          m01*m01 + m11*m11 + m21*m21 +
                          m02*m02 + m12*m12 + m22*m22);
    }

    f32 Matrix3::Determinant() const
    {
        const Matrix2 X{{m11, m12}, {m21, m22}};
        const Matrix2 Y{{m01, m02}, {m21, m22}};
        const Matrix2 Z{{m01, m02}, {m11, m12}};
        return m00 * X.Determinant() - m10 * Y.Determinant() + m20 * Z.Determinant();
    }

    Matrix3 Matrix3::Inverted() const
    {
        return Identity;
    }


    Vector3 Matrix3::operator*(const Vector3& Vec) const
    {
        const Vector3 Row0 = GetRow(0);
        const Vector3 Row1 = GetRow(1);
        const Vector3 Row2 = GetRow(2);
        const Vector3 Result = {Row0.Dot(Vec), Row1.Dot(Vec), Row2.Dot(Vec)};
        return Result;
    }

    Matrix3 Matrix3::operator*(const Matrix3& Mat) const
    {
        const Vector3 Row0 = GetRow(0);
        const Vector3 Row1 = GetRow(1);
        const Vector3 Row2 = GetRow(2);

        const Vector3& Col0 = Mat.Columns[0];
        const Vector3& Col1 = Mat.Columns[1];
        const Vector3& Col2 = Mat.Columns[2];

        const Vector3 NewCol0 { Row0.Dot(Col0), Row1.Dot(Col0), Row2.Dot(Col0) };
        const Vector3 NewCol1 { Row0.Dot(Col1), Row1.Dot(Col1), Row2.Dot(Col1) };
        const Vector3 NewCol2 { Row0.Dot(Col2), Row1.Dot(Col2), Row2.Dot(Col2) };
        return { NewCol0, NewCol1, NewCol2 };
    }

    Matrix3& Matrix3::operator*(const f32 Scalar)
    {
        Columns[0] *= Scalar;
        Columns[1] *= Scalar;
        Columns[2] *= Scalar;
        return *this;
    }

    Vector3& Matrix3::operator[](const size_t i)
    {
        NOVA_ASSERT(i < 3, "Cannot access Mat3 element: index out of bounds.");
        return Columns[i];
    }

    const Vector3& Matrix3::operator[](const size_t i) const
    {
        NOVA_ASSERT(i < 3, "Cannot access Mat3 element: index out of bounds.");
        return Columns[i];
    }

    Vector3 Matrix3::GetRow(const size_t i) const
    {
        NOVA_ASSERT(i < 3, "Cannot access Mat3 element: index out of bounds.");
        switch (i)
        {
        case 0: return { Columns[0].x, Columns[1].x, Columns[2].x };
        case 1: return { Columns[0].y, Columns[1].y, Columns[2].y };
        case 2: return { Columns[0].z, Columns[1].z, Columns[2].z };
        default: throw;
        }
    }

    void Matrix3::Translate(const Vector2& Vec)
    {
        *this = Math::Translate(*this, Vec);
    }

    void Matrix3::Rotate(const Vector3& Axis, const f32 Radians)
    {
        *this = Math::RotateAxisAngle(*this, Axis, Radians);
    }

    void Matrix3::RotateDegrees(const Vector3& Axis, const f32 Degrees)
    {
        *this = Math::RotateAxisAngleDegrees(*this, Axis, Degrees);
    }

    void Matrix3::Scale(const f32 Scalar)
    {
        *this = Math::Scale(*this, Scalar);
    }

    void Matrix3::Scale(const Vector3& Scale)
    {
        *this = Math::Scale(*this, Scale);
    }
}
