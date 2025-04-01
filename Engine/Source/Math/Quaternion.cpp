#include "Quaternion.h"
#include "Functions.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Nova
{
    Quaternion Quaternion::One = { 1.0f, 1.0f, 1.0f, 1.0f };
    Quaternion Quaternion::Identity = { 1.0f, 0.0f, 0.0f, 0.0f };

    Quaternion::Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f)
    {
    }

    Quaternion::Quaternion(f32 W, f32 X, f32 Y, f32 Z): w(W), x(X), y(Y), z(Z){}
    
    

    Quaternion Quaternion::operator+(const Quaternion& Other) const
    {
        return { w + Other.w, x + Other.x, y + Other.y, z + Other.z };
    }

    Quaternion Quaternion::operator*(const Quaternion& Other) const
    {
        return { w * Other.w - x * Other.x - y * Other.y - z * Other.z,
                    w * Other.x + x * Other.w + y * Other.z - z * Other.y,
                    w * Other.y - x * Other.z + y * Other.w + z * Other.x,
                    w * Other.z + x * Other.y - y * Other.x + z * Other.w };
    }

    Quaternion Quaternion::Multiply(f32 Other) const
    {
        return { w * Other, x * Other, y * Other, z * Other };
    }
    
    Quaternion Quaternion::operator*(f32 Other) const
    {
        return Multiply(Other);
    }

    Vector4 Quaternion::Multiply(const Vector4& Vector) const
    {
        return Vector4::Zero;
    }

    Vector4 Quaternion::operator*(const Vector4& Vector) const
    {
        return Multiply(Vector);
    }

    f32 Quaternion::Magnitude() const
    {
        return Math::Sqrt(x*x + y*y + z*z + w*w);
    }

    Quaternion Quaternion::Normalized() const
    {
        return { w / Magnitude(), y / Magnitude(), z / Magnitude(), x / Magnitude() };
    }

    Quaternion Quaternion::Conjugated() const
    {
        return { w, -x, -y, -z };
    }

    Quaternion Quaternion::Inverted() const
    {
        return Conjugated().Normalized();
    }

    f32 Quaternion::Dot(const Quaternion& Other)
    {
        return 0.0f;
    }

    

    Quaternion Quaternion::Multiply(const Quaternion& Other) const
    {
        Quaternion Result;
        Result.x =  x * Other.w +
                    y * Other.z -
                    z * Other.y +
                    w * Other.x;

        Result.y =  -x * Other.z +
                    y * Other.w +
                    z * Other.x +
                    w * Other.y;

        Result.z =  x * Other.y -
                    y * Other.x +
                    z * Other.w +
                    w * Other.z;
        
        Result.w =  -x * Other.x -
                    y * Other.y -
                    z * Other.z +
                    w * Other.w;
        return Result;
    }

    
}
