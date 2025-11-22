#include "Std140Buffer.h"

#include "Math/Matrix2.h"
#include "Math/Matrix3x4.h"
#include "Math/Matrix4.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

namespace Nova
{
    Std140Buffer::Std140Buffer(void* where, const size_t size) : m_Stream(BufferView((uint8_t*)where, size))
    {
    }

    void Std140Buffer::WriteVector2(const Vector2& vector)
    {
        m_Stream.WriteRaw(vector.ValuePtr(), sizeof(Vector2));
    }

    void Std140Buffer::WriteVector3Aligned(const Vector3& vector)
    {
        Vector4 asVec4(vector, 0.0f);
        m_Stream.WriteRaw(asVec4.ValuePtr(), sizeof(Vector4));
    }

    void Std140Buffer::WriteVector4(const Vector4& vector)
    {
        m_Stream.WriteRaw(vector.ValuePtr(), sizeof(Vector4));
    }

    void Std140Buffer::WriteMatrix2(const Matrix2& matrix)
    {
        m_Stream.WriteRaw(matrix.ValuePtr(), sizeof(Matrix2));
    }

    void Std140Buffer::WriteMatrix3Aligned(const Matrix3& matrix)
    {
        const Matrix3x4 m(matrix);
        m_Stream.WriteRaw(m.ValuePtr(), sizeof(Matrix3x4));
    }

    void Std140Buffer::WriteMatrix4(const Matrix4& matrix)
    {
        m_Stream.WriteRaw(matrix.ValuePtr(), sizeof(Matrix4));
    }
}
