#include "Matrix3x4.h"
#include "Matrix3.h"

namespace Nova
{
    Matrix3x4::Matrix3x4()
    {
        Columns[0] = Vector4::Zero;
        Columns[1] = Vector4::Zero;
        Columns[2] = Vector4::Zero;
    }

    Matrix3x4::Matrix3x4(const Matrix3& Matrix)
    {
        Columns[0] = Vector4(Matrix[0]);
        Columns[1] = Vector4(Matrix[1]);
        Columns[2] = Vector4(Matrix[2]);
    }
}
