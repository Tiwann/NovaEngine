#pragma once
#include "Vector4.h"
#include "Matrix3.h"

namespace Nova
{
    class Matrix3x4
    {
    public:
        Matrix3x4();
        explicit Matrix3x4(const Matrix3& Matrix);
    private:
        union
        {
            Vector4 Columns[3];
            struct { f32 m00, m10, m20, m01, m11, m21, m02, m12, m22, m03, m13, m23; };
        };
    };

}
