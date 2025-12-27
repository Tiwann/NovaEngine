#include "Matrix4.h"
#include "Functions.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Runtime/Assertion.h"
#include <algorithm>

#include "Matrix3.h"

namespace Nova
{
    const Matrix4 Matrix4::Identity = Matrix4();
    const Matrix4 Matrix4::One = { Vector4(1.0f), Vector4(1.0f), Vector4(1.0f), Vector4(1.0f) };
    
    Matrix4::Matrix4()
    {
        columns[0] = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
        columns[1] = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
        columns[2] = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
        columns[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    Matrix4::Matrix4(const Vector4& Col1, const Vector4& Col2, const Vector4& Col3, const Vector4& Col4)
    {
        columns[0] = Col1;
        columns[1] = Col2;
        columns[2] = Col3;
        columns[3] = Col4;
    }

    const float* Matrix4::ValuePtr() const
    {
        return (const float*)this;
    }

    float Matrix4::Magnitude() const
    {
        return Math::Sqrt(m00*m00 + m10*m10 + m20*m20 +
                          m01*m01 + m11*m11 + m21*m21 +
                          m02*m02 + m12*m12 + m22*m22);
    }

    float Matrix4::Determinant() const
    {
        //const Matrix2 X{{m11, m12}, {m21, m22}};
        //const Matrix2 Y{{m01, m02}, {m21, m22}};
        //const Matrix2 Z{{m01, m02}, {m11, m12}};
        //return m00 * X.Determinant() - m10 * Y.Determinant() + m20 * Z.Determinant();
        return 0.0f;
    }

    Matrix4 Matrix4::Inverted() const
    {
	    const float Coef00 = columns[2].z * columns[3].w - columns[3].z * columns[2].w;
	    const float Coef02 = columns[1].z * columns[3].w - columns[3].z * columns[1].w;
	    const float Coef03 = columns[1].z * columns[2].w - columns[2].z * columns[1].w;

	    const float Coef04 = columns[2].y * columns[3].w - columns[3].y * columns[2].w;
	    const float Coef06 = columns[1].y * columns[3].w - columns[3].y * columns[1].w;
	    const float Coef07 = columns[1].y * columns[2].w - columns[2].y * columns[1].w;

	    const float Coef08 = columns[2].y * columns[3].z - columns[3].y * columns[2].z;
	    const float Coef10 = columns[1].y * columns[3].z - columns[3].y * columns[1].z;
	    const float Coef11 = columns[1].y * columns[2].z - columns[2].y * columns[1].z;

	    const float Coef12 = columns[2].x * columns[3].w - columns[3].x * columns[2].w;
	    const float Coef14 = columns[1].x * columns[3].w - columns[3].x * columns[1].w;
	    const float Coef15 = columns[1].x * columns[2].w - columns[2].x * columns[1].w;

	    const float Coef16 = columns[2].x * columns[3].z - columns[3].x * columns[2].z;
	    const float Coef18 = columns[1].x * columns[3].z - columns[3].x * columns[1].z;
	    const float Coef19 = columns[1].x * columns[2].z - columns[2].x * columns[1].z;

	    const float Coef20 = columns[2].x * columns[3].y - columns[3].x * columns[2].y;
	    const float Coef22 = columns[1].x * columns[3].y - columns[3].x * columns[1].y;
	    const float Coef23 = columns[1].x * columns[2].y - columns[2].x * columns[1].y;

	    const Vector4 Fac0(Coef00, Coef00, Coef02, Coef03);
	    const Vector4 Fac1(Coef04, Coef04, Coef06, Coef07);
	    const Vector4 Fac2(Coef08, Coef08, Coef10, Coef11);
	    const Vector4 Fac3(Coef12, Coef12, Coef14, Coef15);
	    const Vector4 Fac4(Coef16, Coef16, Coef18, Coef19);
	    const Vector4 Fac5(Coef20, Coef20, Coef22, Coef23);

	    const Vector4 Vec0(columns[1].x, columns[0].x, columns[0].x, columns[0].x);
	    const Vector4 Vec1(columns[1].y, columns[0].y, columns[0].y, columns[0].y);
	    const Vector4 Vec2(columns[1].z, columns[0].z, columns[0].z, columns[0].z);
	    const Vector4 Vec3(columns[1].w, columns[0].w, columns[0].w, columns[0].w);

	    const Vector4 Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
	    const Vector4 Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
	    const Vector4 Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
	    const Vector4 Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

	    const Vector4 SignA(+1, -1, +1, -1);
	    const Vector4 SignB(-1, +1, -1, +1);
	    const Matrix4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

	    const Vector4 Row0(Inverse[0].x, Inverse[1].x, Inverse[2].x, Inverse[3].x);

	    const Vector4 Dot0(columns[0] * Row0);
	    const float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

	    const float OneOverDeterminant = 1.0f / Dot1;

	    return Inverse * OneOverDeterminant;
    }

    Matrix4 Matrix4::Transposed() const
    {
    	Matrix4 Result = *this;
    	std::swap(Result[0].y, Result[1].x);
    	std::swap(Result[0].z, Result[2].x);
    	std::swap(Result[0].w, Result[3].x);
    	std::swap(Result[1].z, Result[2].y);
    	std::swap(Result[1].w, Result[3].y);
    	std::swap(Result[2].w, Result[3].z);
    	return Result;
    }

    Vector4 Matrix4::operator*(const Vector4& Vec) const
    {
    	const Vector4 Row0 = GetRow(0);
    	const Vector4 Row1 = GetRow(1);
    	const Vector4 Row2 = GetRow(2);
    	const Vector4 Row3 = GetRow(3);
    	const Vector4 Result = { Row0.Dot(Vec), Row1.Dot(Vec), Row2.Dot(Vec), Row3.Dot(Vec) };
    	return Result;
    }

    Vector3 Matrix4::operator*(const Vector3& Vec) const
    {
    	const Vector4 Result = operator*(Vector4(Vec, 1.0f));
    	return Vector3(Result);
    }

    Vector2 Matrix4::operator*(const Vector2& Vec) const
    {
    	const Vector3 Result = operator*(Vector3(Vec, 0.0f));
    	return Vector2(Result);
    }

	Matrix4 Matrix4::operator*(const Matrix4& Mat) const
    {
    	Vector4 Row0 = GetRow(0);
    	Vector4 Row1 = GetRow(1);
    	Vector4 Row2 = GetRow(2);
    	Vector4 Row3 = GetRow(3);

    	const Vector4& Col0 = Mat.columns[0];
    	const Vector4& Col1 = Mat.columns[1];
    	const Vector4& Col2 = Mat.columns[2];
    	const Vector4& Col3 = Mat.columns[3];

    	const Vector4 NewCol0 { Row0.Dot(Col0), Row1.Dot(Col0), Row2.Dot(Col0), Row3.Dot(Col0) };
    	const Vector4 NewCol1 { Row0.Dot(Col1), Row1.Dot(Col1), Row2.Dot(Col1), Row3.Dot(Col1) };
    	const Vector4 NewCol2 { Row0.Dot(Col2), Row1.Dot(Col2), Row2.Dot(Col2), Row3.Dot(Col2) };
    	const Vector4 NewCol3 { Row0.Dot(Col3), Row1.Dot(Col3), Row2.Dot(Col3), Row3.Dot(Col3) };
    	return { NewCol0, NewCol1, NewCol2, NewCol3 };
    }

    Vector4& Matrix4::operator[](const size_t i)
    {
        NOVA_ASSERT(i < 4, "Cannot access Mat4 element: index out of bounds.");
        return columns[i];
    }

    const Vector4& Matrix4::operator[](const size_t i) const
    {
        NOVA_ASSERT(i < 4, "Cannot access Mat4 element: index out of bounds.");
        return columns[i];
    }

	Vector4 Matrix4::GetRow(const size_t i) const
    {
    	NOVA_ASSERT(i < 4, "Cannot access Mat4 element: index out of bounds.");
    	switch (i)
    	{
    	case 0: return { columns[0].x, columns[1].x, columns[2].x, columns[3].x };
    	case 1: return { columns[0].y, columns[1].y, columns[2].y, columns[3].y };
    	case 2: return { columns[0].z, columns[1].z, columns[2].z, columns[3].z };
    	case 3: return { columns[0].w, columns[1].w, columns[2].w, columns[3].w };
    	default: throw;
    	}
    }


    Matrix4 Matrix4::operator*(const float Scalar) const
    {
    	Matrix4 Result = *this;
        Result.columns[0] *= Scalar;
        Result.columns[1] *= Scalar;
        Result.columns[2] *= Scalar;
        Result.columns[3] *= Scalar;
        return  Result;
    }


    void Matrix4::Rotate(const Vector3& Axis, const float Radians)
    {
        *this = Math::RotateAxisAngle(*this, Axis, Radians);
    }

    void Matrix4::Rotate(const Vector3& EulerAngles)
    {
    	*this = Math::RotateEulerAngles(*this, EulerAngles);
    }

    void Matrix4::Rotate(const Quaternion& Rotation)
    {
    	const Matrix4 RotationMatrix = Rotation.ToMatrix4();
    	*this = RotationMatrix * (*this);
    }

    void Matrix4::RotateDegrees(const Vector3& EulerAnglesDegrees)
    {
        *this = Math::RotateEulerAnglesDegrees(*this, EulerAnglesDegrees);
    }

    void Matrix4::RotateDegrees(const Vector3& Axis, const float Degrees)
    {
        *this = Math::RotateAxisAngleDegrees(*this, Axis, Degrees);
    }

    void Matrix4::Scale(const float Scalar)
    {
        *this = Math::Scale(*this, Scalar);
    }

    void Matrix4::Scale(const Vector3& Scale)
    {
        *this = Math::Scale(*this, Scale);
    }

    void Matrix4::Translate(const Vector3& Translation)
    {
        *this = Math::Translate(*this, Translation);
    }

    void Matrix4::Decompose(Vector3* position, Quaternion* rotation, Vector3* scale) const
    {
	    if(!(position || rotation || scale))
	    	return;

	    if (position)
	    	*position = Vector3(columns[3].x, columns[3].y, columns[3].z);

	    Vector3 newScale;
	    newScale.x = Vector3(columns[0]).Magnitude();
	    newScale.y = Vector3(columns[1]).Magnitude();
	    newScale.z = Vector3(columns[2]).Magnitude();

    	if (Determinant() < 0) newScale = -newScale;

	    if (scale)
		    *scale = newScale;


	    if (!rotation)
		    return;

	    if (newScale.x == 0.0f || newScale.y == 0.0f || newScale.z == 0.0f)
	    {
		    *rotation = Quaternion::Identity;
		    return;
	    }

	    Vector3 xAxis = Vector3(columns[0]) / newScale.x;
	    Vector3 yAxis = Vector3(columns[1]) / newScale.y;
	    Vector3 zAxis = Vector3(columns[2]) / newScale.z;

	    xAxis = xAxis.Normalized();
	    yAxis = (yAxis - xAxis * xAxis.Dot(yAxis)).Normalized();
	    zAxis = xAxis.Cross(yAxis);

	    Matrix3 rotationMatrix(xAxis, yAxis, zAxis);

	    *rotation = Quaternion::FromMatrix(rotationMatrix);
    }


    Matrix4 Matrix4::TRS(const Vector3& Position, const Vector3& EulerAnglesDegrees, const Vector3& Scale)
    {
    	Matrix4 Result;
    	Result.Scale(Scale);
    	Result.RotateDegrees(EulerAnglesDegrees);
    	Result.Translate(Position);
    	return Result;
    }

    Matrix4 Matrix4::TRS(const Vector3& Position, const Quaternion& Orientation, const Vector3& Scale)
    {
    	Matrix4 Result = Matrix4::Identity;
    	Result.Scale(Scale);
    	Result.Rotate(Orientation);
    	Result.Translate(Position);
    	return Result;
    }
}
