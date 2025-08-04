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

    Quaternion::Quaternion(const float w, const float x, const float y, const float z): w(w), x(x), y(y), z(z){}

    Quaternion::Quaternion(const Quaternion& other) : w(other.w), x(other.x), y(other.y), z(other.z)
    {

    }

    bool Quaternion::operator!=(const Quaternion& other) const
    {
        return !operator==(other);
    }

    bool Quaternion::operator==(const Quaternion& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    Quaternion Quaternion::operator+(const Quaternion& other) const
    {
        return { w + other.w, x + other.x, y + other.y, z + other.z };
    }

    Quaternion Quaternion::operator-(const Quaternion& other) const
    {
        return { w - other.w, x - other.x, y - other.y, z - other.z };
    }
    

    Quaternion& Quaternion::operator+=(const Quaternion& other)
    {
        w += other.w;
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Quaternion& Quaternion::operator-=(const Quaternion& other)
    {
        w -= other.w;
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Quaternion Quaternion::operator*(const Quaternion& other) const
    {
        return {  w * other.w - x * other.x - y * other.y - z * other.z,
                    w * other.x + x * other.w + y * other.z - z * other.y,
                    w * other.y - x * other.z + y * other.w + z * other.x,
                    w * other.z + x * other.y - y * other.x + z * other.w };
    }

    Quaternion Quaternion::operator*(const float other) const
    {
        return { w * other, x * other, y * other, z * other };
    }

    Quaternion& Quaternion::operator*=(const Quaternion& other)
    {
        *this = operator*(other);
        return *this;
    }

    Quaternion& Quaternion::operator*=(const float other)
    {
        *this = operator*(other);
        return *this;
    }

    Quaternion Quaternion::operator/(const float other) const
    {
        return { w / other, x / other, y / other, z / other };
    }

    Vector3 Quaternion::operator*(const Vector3& other) const
    {
        const Vector3 QuatVector(x, y, z);
        const Vector3 UV(QuatVector.Cross(other));
        const Vector3 UUV(QuatVector.Cross(UV));
        return other + (UV * w + UUV) * 2.0f;
    }

    Vector4 Quaternion::operator*(const Vector4& other) const
    {
        return Vector4(operator*(Vector3(other)), other.w);
    }

    Matrix4 Quaternion::operator*(const Matrix4& other) const
    {
        return ToMatrix4() * other;
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

    void Quaternion::ToAxisAngle(Vector3& axis, float& radians) const
    {
        radians = Math::Acos(w) * 2.0f;
        const float sin = Math::Sin(radians * 0.5f);
        axis.x = x / sin;
        axis.y = y / sin;
        axis.z = z / sin;
    }

    void Quaternion::ToAxisAngleDegrees(Vector3& axis, float& degrees) const
    {
        float radians = 0.0f;
        ToAxisAngle(axis, radians);
        degrees = Math::Degrees(radians);
    }

    Quaternion Quaternion::FromAxisAngle(const Vector3& axis, const float radians)
    {
        const Vector3 a = Math::Normalize(axis);
        const float halfSinr = Math::Sin(radians * 0.5f);

        Quaternion result;
        result.w = Math::Cos(radians * 0.5f);
        result.x = a.x * halfSinr;
        result.y = a.y * halfSinr;
        result.z = a.z * halfSinr;
        return result;
    }

    Quaternion Quaternion::FromAxisAngleDegrees(const Vector3& axis, const float degrees)
    {
        return FromAxisAngle(axis, Math::Radians(degrees));
    }

    Quaternion Quaternion::FromEuler(const Vector3& eulerAngles)
    {
        const float cosX = Math::Cos(eulerAngles.x * 0.5f);
        const float sinX = Math::Sin(eulerAngles.x * 0.5f);
        const float cosY = Math::Cos(eulerAngles.y * 0.5f);
        const float sinY = Math::Sin(eulerAngles.y * 0.5f);
        const float cosZ = Math::Cos(eulerAngles.z * 0.5f);
        const float sinZ = Math::Sin(eulerAngles.z * 0.5f);

        Quaternion result;
        result.w = cosX * cosY * cosZ + sinX * sinY * sinZ;
        result.x = sinX * cosY * cosZ - cosX * sinY * sinZ;
        result.y = cosX * sinY * cosZ + sinX * cosY * sinZ;
        result.z = cosX * cosY * sinZ - sinX * sinY * cosZ;
        return result;
    }

    Quaternion Quaternion::FromEulerDegrees(const Vector3& eulerAnglesDegrees)
    {
        return FromEuler(eulerAnglesDegrees.Apply(Math::Radians));
    }

    float Quaternion::Magnitude() const
    {
        return Math::Sqrt(x*x + y*y + z*z + w*w);
    }

    Quaternion Quaternion::Normalized() const
    {
        const float magnitude = Magnitude();
        if (magnitude <= 0) return Quaternion();
        const float inverseMagnitude = 1.0f / magnitude;
        return { w * inverseMagnitude, y * inverseMagnitude, z * inverseMagnitude, x * inverseMagnitude };
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

    Quaternion Quaternion::Cross(const Quaternion& other) const
    {
        return Quaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y + y * other.w + z * other.x - x * other.z,
            w * other.z + z * other.w + x * other.y - y * other.x);
    }

    float Quaternion::Dot(const Quaternion& other) const
    {
        return Math::Sqrt(x * other.x + y * other.y + z * other.z + w * other.w);
    }
    

}
