#pragma once
#include "Runtime/Types.h"
#include <CoreExport.h>

namespace Nova
{
    struct Vector3;
    struct Vector4;
    
    class CORE_API Quaternion
    {
    public:
        Quaternion();
        Quaternion(f32 W, f32 X, f32 Y, f32 Z);
        
        f32 Magnitude() const;
        Quaternion Normalized() const;
        Quaternion Conjugated() const;
        Quaternion Inverted() const;
        f32 Dot(const Quaternion& Other);

        
        Quaternion operator+(const Quaternion& Other) const;
        
        Quaternion Multiply(const Quaternion& Other) const;
        Quaternion Multiply(f32 Other) const;
        Vector4 Multiply(const Vector4& Vector) const;
        Quaternion operator*(const Quaternion& Other) const;
        Quaternion operator*(f32 Other) const;
        Vector4 operator*(const Vector4& Vector) const;
        
        
        static Quaternion One;
        static Quaternion Identity;
    private:
        f32 w, x, y, z;
    };
}

