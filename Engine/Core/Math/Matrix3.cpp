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
        Columns[0] = {};
        Columns[1] = {};
        Columns[2] = {};
        m00 = m11 = m22 = 1.0f;
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

    Vector3 Matrix3::Multiply(const Vector3& Vec) const
    {
        return Columns[0] * Vec.x + Columns[1] * Vec.y + Columns[2] * Vec.z;
    }

    Vector3 Matrix3::operator*(const Vector3& Vec) const
    {
        return Multiply(Vec);
    }

    Matrix3 Matrix3::Multiply(const Matrix3& Mat) const
    {
        Vector3 Col1 { Mat.m00 * m00 + Mat.m01 * m10 + Mat.m02 * m20,
                       Mat.m00 * m01 + Mat.m01 * m11 + Mat.m02 * m21,
                       Mat.m00 * m02 + Mat.m01 * m12 + Mat.m02 * m22 };
        
        Vector3 Col2 { Mat.m10 * m00 + Mat.m11 * m10 + Mat.m12 * m20,
                       Mat.m10 * m01 + Mat.m11 * m11 + Mat.m12 * m21,
                       Mat.m10 * m02 + Mat.m11 * m12 + Mat.m12 * m22 };

        Vector3 Col3 { Mat.m20 * m00 + Mat.m21 * m10 + Mat.m22 * m20,
                       Mat.m20 * m01 + Mat.m21 * m11 + Mat.m22 * m21,
                       Mat.m20 * m02 + Mat.m21 * m12 + Mat.m22 * m22 };
        
        return { Col1, Col2, Col3 };
    }

    Matrix3 Matrix3::operator*(const Matrix3& Mat) const
    {
        return Multiply(Mat);
    }

    Vector3& Matrix3::operator[](size_t i)
    {
        Assert(i < 2, "Cannot access Mat2 element: index out of bounds.");
        return Columns[i];
    }

    const Vector3& Matrix3::operator[](size_t i) const
    {
        Assert(i < 2, "Cannot access Mat2 element: index out of bounds.");
        return Columns[i];
    }

    Matrix3& Matrix3::operator*(f32 Scalar)
    {
        Columns[0] *= Scalar;
        Columns[1] *= Scalar;
        Columns[2] *= Scalar;
        return *this;
    }

    void Matrix3::Rotate(f32 Radians, const Vector3& Axis)
    {
        *this = Math::Rotate(*this, Axis, Radians);
    }

    void Matrix3::RotateDegrees(f32 Degrees, const Vector3& Axis)
    {
        *this = Math::RotateDegrees(*this, Axis, Degrees);
    }

    void Matrix3::Scale(f32 Scalar)
    {
        *this = Math::Scale(*this, Scalar);
    }

    void Matrix3::Scale(const Vector3& Scale)
    {
        *this = Math::Scale(*this, Scale);
    }
}
