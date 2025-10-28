#pragma once
#include "BufferView.h"
#include "Serialization/MemoryStream.h"

namespace Nova
{
    struct Vector3;
    struct Vector4;
    struct Vector2;

    class Matrix2;

    class Std140Buffer
    {
    public:
        Std140Buffer(void* where, size_t size);
        ~Std140Buffer() = default;

        void WriteVector2(const Vector2& vector);
        void WriteVector3Aligned(const Vector3& vector);
        void WriteVector4(const Vector4& vector);

        void WriteMatrix2(const Matrix2& matrix);
        void WriteMatrix3Aligned(const Matrix3& matrix);
        void WriteMatrix4(const Matrix4& matrix);

        size_t Tell() const { return m_Stream.Tell(); }
    private:
        MemoryStream m_Stream;
    };
}