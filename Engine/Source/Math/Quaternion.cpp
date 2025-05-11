#include "Quaternion.h"
#include "Functions.h"
#include "Matrix3.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Nova
{
    Quaternion Quaternion::One = { 1.0f, 1.0f, 1.0f, 1.0f };
    Quaternion Quaternion::Identity = { 1.0f, 0.0f, 0.0f, 0.0f };

    Quaternion::Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f)
    {
    }

    Quaternion::Quaternion(const f32 W, const f32 X, const f32 Y, const f32 Z): w(W), x(X), y(Y), z(Z){}

    Quaternion::Quaternion(const Quaternion& Other) : w(Other.w), x(Other.x), y(Other.y), z(Other.z)
    {

    }

    bool Quaternion::operator!=(const Quaternion& Other) const
    {
        return !operator==(Other);
    }

    bool Quaternion::operator==(const Quaternion& Other) const
    {
        return x == Other.x && y == Other.y && z == Other.z;
    }

    Quaternion Quaternion::operator+(const Quaternion& Other) const
    {
        return { w + Other.w, x + Other.x, y + Other.y, z + Other.z };
    }

    Quaternion Quaternion::operator-(const Quaternion& Other) const
    {
        return { w - Other.w, x - Other.x, y - Other.y, z - Other.z };
    }
    

    Quaternion& Quaternion::operator+=(const Quaternion& Other)
    {
        w += Other.w;
        x += Other.x;
        y += Other.y;
        z += Other.z;
        return *this;
    }

    Quaternion& Quaternion::operator-=(const Quaternion& Other)
    {
        w -= Other.w;
        x -= Other.x;
        y -= Other.y;
        z -= Other.z;
        return *this;
    }

    Quaternion Quaternion::operator*(const Quaternion& Other) const
    {
        return {  w * Other.w - x * Other.x - y * Other.y - z * Other.z,
                    w * Other.x + x * Other.w + y * Other.z - z * Other.y,
                    w * Other.y - x * Other.z + y * Other.w + z * Other.x,
                    w * Other.z + x * Other.y - y * Other.x + z * Other.w };
    }

    Quaternion Quaternion::operator*(const f32 Other) const
    {
        return { w * Other, x * Other, y * Other, z * Other };
    }

    Quaternion& Quaternion::operator*=(const Quaternion& Other)
    {
        *this = operator*(Other);
        return *this;
    }

    Quaternion& Quaternion::operator*=(const f32 Other)
    {
        *this = operator*(Other);
        return *this;
    }

    Quaternion Quaternion::operator/(const f32 Other) const
    {
        return { w / Other, x / Other, y / Other, z / Other };
    }

    Vector3 Quaternion::operator*(const Vector3& Other) const
    {
        const Vector3 QuatVector(x, y, z);
        const Vector3 UV(QuatVector.Cross(Other));
        const Vector3 UUV(QuatVector.Cross(UV));
        return Other + (UV * w + UUV) * 2.0f;
    }

    Vector4 Quaternion::operator*(const Vector4& Other) const
    {
        return Vector4(operator*(Vector3(Other)), Other.w);
    }

    Matrix4 Quaternion::operator*(const Matrix4& Other) const
    {
        return ToMatrix4() * Other;
    }

    Matrix4 Quaternion::ToMatrix4() const
    {
        const f32 xx = x * x;
        const f32 yy = y * y;
        const f32 zz = z * z;
        const f32 xy = x * y;
        const f32 xz = x * z;
        const f32 yz = y * z;
        const f32 wx = w * x;
        const f32 wy = w * y;
        const f32 wz = w * z;

        Matrix4 Result = Matrix4::Identity;
        Result[0][0] = 1.0f - 2.0f * (yy + zz);
        Result[0][1] = 2.0f * (xy + wz);
        Result[0][2] = 2.0f * (xz - wy);
        Result[1][0] = 2.0f * (xy - wz);
        Result[1][1] = 1.0f - 2.0f * (xx + zz);
        Result[1][2] = 2.0f * (yz + wx);
        Result[2][0] = 2.0f * (xz + wy);
        Result[2][1] = 2.0f * (yz - wx);
        Result[2][2] = 1.0f - 2.0f * (xx + yy);
        return Result;
    }

    Matrix3 Quaternion::ToMatrix3() const
    {
        const f32 xx = x * x;
        const f32 yy = y * y;
        const f32 zz = z * z;
        const f32 xy = x * y;
        const f32 xz = x * z;
        const f32 yz = y * z;
        const f32 wx = w * x;
        const f32 wy = w * y;
        const f32 wz = w * z;

        Matrix3 Result = Matrix3::Identity;
        Result[0][0] = 1.0f - 2.0f * (yy + zz);
        Result[0][1] = 2.0f * (xy + wz);
        Result[0][2] = 2.0f * (xz - wy);
        Result[1][0] = 2.0f * (xy - wz);
        Result[1][1] = 1.0f - 2.0f * (xx + zz);
        Result[1][2] = 2.0f * (yz + wx);
        Result[2][0] = 2.0f * (xz + wy);
        Result[2][1] = 2.0f * (yz - wx);
        Result[2][2] = 1.0f - 2.0f * (xx + yy);
        return Result;
    }

    Quaternion Quaternion::Euler(const Vector3& EulerAngles)
    {
        const f32 CosX = Math::Cos(EulerAngles.x * 0.5f);
        const f32 SinX = Math::Sin(EulerAngles.x * 0.5f);
        const f32 CosY = Math::Cos(EulerAngles.y * 0.5f);
        const f32 SinY = Math::Sin(EulerAngles.y * 0.5f);
        const f32 CosZ = Math::Cos(EulerAngles.z * 0.5f);
        const f32 SinZ = Math::Sin(EulerAngles.z * 0.5f);

        Quaternion Result;
        Result.w = CosX * CosY * CosZ + SinX * SinY * SinZ;
        Result.x = SinX * CosY * CosZ - CosX * SinY * SinZ;
        Result.y = CosX * SinY * CosZ + SinX * CosY * SinZ;
        Result.z = CosX * CosY * SinZ - SinX * SinY * CosZ;
        return Result;
    }

    Quaternion Quaternion::Euler(const f32 X, const f32 Y, const f32 Z)
    {
        return Euler(Vector3(X, Y, Z));
    }

    Quaternion Quaternion::EulerDegrees(const Vector3& EulerAnglesDegrees)
    {
        return Euler(EulerAnglesDegrees.Apply(Math::Radians));
    }

    Quaternion Quaternion::EulerDegrees(const f32 X, const f32 Y, const f32 Z)
    {
        return Euler(Vector3(X, Y, Z).Apply(Math::Radians));
    }

    f32 Quaternion::Magnitude() const
    {
        return Math::Sqrt(x*x + y*y + z*z + w*w);
    }

    Quaternion Quaternion::Normalized() const
    {
        const f32 CachedMagnitude = Magnitude();
        if (CachedMagnitude <= 0) return Quaternion();
        const f32 InverseMagnitude = 1.0f / CachedMagnitude;
        return { w * InverseMagnitude, y * InverseMagnitude, z * InverseMagnitude, x * InverseMagnitude };
    }

    Quaternion Quaternion::Conjugated() const
    {
        return { w, -x, -y, -z };
    }

    Quaternion Quaternion::Inverted() const
    {
        const f32 Dot = this->Dot(*this);
        const Quaternion Conjugate = this->Conjugated();
        return Conjugate / Dot;
    }

    Quaternion Quaternion::Cross(const Quaternion& Other) const
    {
        return Quaternion(
            w * Other.w - x * Other.x - y * Other.y - z * Other.z,
            w * Other.x + x * Other.w + y * Other.z - z * Other.y,
            w * Other.y + y * Other.w + z * Other.x - x * Other.z,
            w * Other.z + z * Other.w + x * Other.y - y * Other.x);
    }

    f32 Quaternion::Dot(const Quaternion& Other) const
    {
        return Math::Sqrt(x * Other.x + y * Other.y + z * Other.z + w * Other.w);
    }
    

}
