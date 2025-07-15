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

    Quaternion::Quaternion(const float w, const float X, const float Y, const float Z): w(w), x(X), y(Y), z(Z){}

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

    Quaternion Quaternion::operator*(const float Other) const
    {
        return { w * Other, x * Other, y * Other, z * Other };
    }

    Quaternion& Quaternion::operator*=(const Quaternion& Other)
    {
        *this = operator*(Other);
        return *this;
    }

    Quaternion& Quaternion::operator*=(const float Other)
    {
        *this = operator*(Other);
        return *this;
    }

    Quaternion Quaternion::operator/(const float Other) const
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
        const float xx = x * x;
        const float yy = y * y;
        const float zz = z * z;
        const float xy = x * y;
        const float xz = x * z;
        const float yz = y * z;
        const float wx = w * x;
        const float wy = w * y;
        const float wz = w * z;

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
        const float xx = x * x;
        const float yy = y * y;
        const float zz = z * z;
        const float xy = x * y;
        const float xz = x * z;
        const float yz = y * z;
        const float wx = w * x;
        const float wy = w * y;
        const float wz = w * z;

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

    Vector3 Quaternion::ToEuler() const
    {
        const float sinr_cosp = 2.0f * (w * x + y * z);
        const float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
        const float EulerX = Math::Atan2(sinr_cosp, cosr_cosp);

        const float sinp = Math::Sqrt(1.0f + 2.0f * (w * y - x * z));
        const float cosp = Math::Sqrt(1.0f - 2.0f * (w * y - x * z));
        const float EulerY = 2.0f * Math::Atan2(sinp, cosp) - Math::Pi * 0.5f;

        const float siny_cosp = 2.0f * (w * z + x * y);
        const float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
        const float EulerZ = Math::Atan2(siny_cosp, cosy_cosp);

        return { EulerX, EulerY, EulerZ };
    }

    Vector3 Quaternion::ToEulerDegrees() const
    {
        return ToEuler().Apply(Math::Degrees);
    }

    Quaternion Quaternion::FromAxisAngle(const Vector3& Axis, const float Radians)
    {
        const Vector3 A = Math::Normalize(Axis);
        Quaternion Result;
        Result.w = Math::Cos(Radians * 0.5f);
        Result.x = A.x * Math::Sin(Radians * 0.5f);
        Result.y = A.y * Math::Sin(Radians * 0.5f);
        Result.z = A.z * Math::Sin(Radians * 0.5f);
        return Result;
    }

    Quaternion Quaternion::FromAxisAngleDegrees(const Vector3& Axis, const float Degrees)
    {
        return FromAxisAngle(Axis, Math::Radians(Degrees));
    }

    Quaternion Quaternion::FromEuler(const Vector3& EulerAngles)
    {
        const float CosX = Math::Cos(EulerAngles.x * 0.5f);
        const float SinX = Math::Sin(EulerAngles.x * 0.5f);
        const float CosY = Math::Cos(EulerAngles.y * 0.5f);
        const float SinY = Math::Sin(EulerAngles.y * 0.5f);
        const float CosZ = Math::Cos(EulerAngles.z * 0.5f);
        const float SinZ = Math::Sin(EulerAngles.z * 0.5f);

        Quaternion Result;
        Result.w = CosX * CosY * CosZ + SinX * SinY * SinZ;
        Result.x = SinX * CosY * CosZ - CosX * SinY * SinZ;
        Result.y = CosX * SinY * CosZ + SinX * CosY * SinZ;
        Result.z = CosX * CosY * SinZ - SinX * SinY * CosZ;
        return Result;
    }

    Quaternion Quaternion::FromEuler(const float X, const float Y, const float Z)
    {
        return FromEuler(Vector3(X, Y, Z));
    }

    Quaternion Quaternion::FromEulerDegrees(const Vector3& EulerAnglesDegrees)
    {
        return FromEuler(EulerAnglesDegrees.Apply(Math::Radians));
    }

    Quaternion Quaternion::FromEulerDegrees(const float X, const float Y, const float Z)
    {
        return FromEuler(Vector3(X, Y, Z).Apply(Math::Radians));
    }

    float Quaternion::Magnitude() const
    {
        return Math::Sqrt(x*x + y*y + z*z + w*w);
    }

    Quaternion Quaternion::Normalized() const
    {
        const float CachedMagnitude = Magnitude();
        if (CachedMagnitude <= 0) return Quaternion();
        const float InverseMagnitude = 1.0f / CachedMagnitude;
        return { w * InverseMagnitude, y * InverseMagnitude, z * InverseMagnitude, x * InverseMagnitude };
    }

    Quaternion Quaternion::Conjugated() const
    {
        return { w, -x, -y, -z };
    }

    Quaternion Quaternion::Inverted() const
    {
        const float Dot = this->Dot(*this);
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

    float Quaternion::Dot(const Quaternion& Other) const
    {
        return Math::Sqrt(x * Other.x + y * Other.y + z * Other.z + w * Other.w);
    }
    

}
