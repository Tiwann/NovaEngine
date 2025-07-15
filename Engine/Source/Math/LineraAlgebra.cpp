#include "LinearAlgebra.h"

namespace Nova
{
    Matrix2 Math::Rotate(const Matrix2& mat, const float radians)
    {
        const Matrix2 rotationMatrix
        {
            Vector2(Cos(radians), Sin(radians)),
            Vector2(-Sin(radians), Cos(radians))
        };
        return rotationMatrix * mat;
    }

    Matrix2 Math::RotateDegrees(const Matrix2& mat, const float degrees)
    {
        return Rotate(mat, Radians(degrees));
    }

    Matrix2 Math::Scale(const Matrix2& mat, const Vector2& scale)
    {
        const Matrix2 scaleMatrix
        {
            Vector2(scale.x, 0.0f),
            Vector2(0.0f, scale.y)
        };
        return scaleMatrix * mat;
    }

    Matrix2 Math::Scale(const Matrix2& mat, const float scale)
    {
        const Matrix2 scaleMatrix
        {
            Vector2(scale, 0.0f),
            Vector2(0.0f, scale)
        };
        return scaleMatrix * mat;
    }

    Matrix3 Math::RotateAxisAngle(const Matrix3& mat, const Vector3& axis, const float angleRadians)
    {
        const Vector3 normalizedAxis = Normalize(axis);
        const float c = Cos(angleRadians);
        const float s = Sin(angleRadians);
        const float invC = 1.0f - c;
        const float xx = normalizedAxis.x * normalizedAxis.x;
        const float yy = normalizedAxis.y * normalizedAxis.y;
        const float zz = normalizedAxis.z * normalizedAxis.z;
        const float xy = normalizedAxis.x * normalizedAxis.y;
        const float xz = normalizedAxis.x * normalizedAxis.z;
        const float yz = normalizedAxis.y * normalizedAxis.z;
        const float x = normalizedAxis.x;
        const float y = normalizedAxis.y;
        const float z = normalizedAxis.z;

        const Matrix3 rotationMatrix
        {
            Vector3(xx * invC + c, xy * invC + z * s, xz * invC - y * s),
            Vector3(xy * invC - z * s, yy * invC + c, yz * invC + x * s),
            Vector3(xz * invC + y * s, yz * invC - x * s, zz * invC + c),
        };
        return rotationMatrix * mat;
    }

    Matrix3 Math::RotateAxisAngleDegrees(const Matrix3& mat, const Vector3& axis, const float angleDegrees)
    {
        return RotateAxisAngle(mat, axis, Radians(angleDegrees));
    }

    Matrix3 Math::RotateEulerAngles(const Matrix3& mat, const Vector3& eulerAnglesRadians)
    {
        Matrix3 result = Matrix3::Identity;
        result.Rotate(Vector3::Forward, eulerAnglesRadians.z);
        result.Rotate(Vector3::Up, eulerAnglesRadians.y);
        result.Rotate(Vector3::Right, eulerAnglesRadians.x);
        return result * mat;
    }

    Matrix3 Math::RotateEulerAnglesDegrees(const Matrix3& mat, const Vector3& eulerAnglesDegrees)
    {
        return RotateEulerAngles(mat, eulerAnglesDegrees.Apply(Radians));
    }

    Matrix3 Math::Scale(const Matrix3& mat, const Vector3& scale)
    {
        const Matrix3 scaleMatrix
        {
            Vector3(scale.x, 0.0f, 0.0f),
            Vector3(0.0f, scale.y, 0.0f),
            Vector3(0.0f, 0.0f, scale.z),
        };
        return scaleMatrix * mat;
    }

    Matrix3 Math::Scale(const Matrix3& mat, float scale)
    {
        const Matrix3 scaleMatrix
        {
            Vector3(scale, 0.0f, 0.0f),
            Vector3(0.0f, scale, 0.0f),
            Vector3(0.0f, 0.0f, scale),
        };
        return scaleMatrix * mat;
    }

    Matrix3 Math::Translate(const Matrix3& mat, const Vector2& translation)
    {
        const float tx = translation.x;
        const float ty = translation.y;

        const Matrix3 translationMatrix = {
            Vector3(1.0f, 0.0f, 0.0f),
            Vector3(0.0f, 1.0f, 0.0f),
            Vector3(tx, ty, 1.0f),
        };
        return translationMatrix * mat;
    }

    Matrix4 Math::RotateAxisAngle(const Matrix4& mat, const Vector3& axis, const float angleRadians)
    {
        const Matrix3 rotationMatrix = RotateAxisAngle(Matrix3::Identity, axis, angleRadians);
        const Matrix4 result
        {
            Vector4(rotationMatrix[0], 0.0f),
            Vector4(rotationMatrix[1], 0.0f),
            Vector4(rotationMatrix[2], 0.0f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f)
        };
        return result * mat;
    }

    Matrix4 Math::RotateAxisAngleDegrees(const Matrix4& mat, const Vector3& axis, const float angleDegrees)
    {
        return RotateAxisAngle(mat, axis, Radians(angleDegrees));
    }

    Matrix4 Math::Scale(const Matrix4& mat, const Vector3& scale)
    {
        const Matrix4 scaleMatrix
        {
            Vector4(scale.x, 0.0f, 0.0, 0.0f),
            Vector4(0.0f, scale.y, 0.0f, 0.0f),
            Vector4(0.0f, 0.0f, scale.z, 0.0f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f),
        };
        return scaleMatrix * mat;
    }

    Matrix4 Math::Scale(const Matrix4& mat, float scale)
    {
        const Matrix4 scaleMatrix =
        {
            Vector4(scale, 0.0f, 0.0, 0.0f),
            Vector4(0.0f, scale, 0.0f, 0.0f),
            Vector4(0.0f, 0.0f, scale, 0.0f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f),
        };
        return scaleMatrix * mat;
    }

    Matrix4 Math::RotateEulerAngles(const Matrix4& mat, const Vector3& eulerAnglesRadians)
    {
        Matrix4 result = Matrix4::Identity;
        result.Rotate(Vector3::Forward, eulerAnglesRadians.z);
        result.Rotate(Vector3::Up, eulerAnglesRadians.y);
        result.Rotate(Vector3::Right, eulerAnglesRadians.x);
        return result * mat;
    }

    Matrix4 Math::RotateEulerAnglesDegrees(const Matrix4& mat, const Vector3& eulerAnglesDegrees)
    {
        return RotateEulerAngles(mat, eulerAnglesDegrees.Apply(Radians));
    }

    Matrix4 Math::Translate(const Matrix4& mat, const Vector3& translation)
    {
        Matrix4 translationMatrix = Matrix4::Identity;
        translationMatrix[3] = Vector4(translation, 1.0f);
        return translationMatrix * mat;
    }

    Matrix4 Math::Perspective(const float fov, const float aspectRatio, const float near, const float far)
    {
        const float scale = 1.0f / Tan(Radians(fov * 0.5f));
        const float m22 = far / (near - far);
        const float m32 = -(far * near) / (far - near);

        return {
            Vector4(scale / aspectRatio, 0.0f,  0.0f,  0.0f),
            Vector4(0.0f,               scale,  0.0f,  0.0f),
            Vector4(0.0f,               0.0f,   m22,   -1.0f),
            Vector4(0.0f,               0.0f,   m32,   0.0f)
        };
    }

    Matrix4 Math::Orthographic(const float width, const float height, const float scale, const float near, const float far)
    {
        const Matrix4 projection
        {
            Vector4(scale * 2.0f / width, 0.0f, 0.0f, 0.0f),
            Vector4(0.0f, scale * 2.0f / height, 0.0f, 0.0f),
            Vector4(0.0f, 0.0f, -1.0f / (far - near), 0.0f),
            Vector4(-0.0f, -0.0f, -near / (far - near), 1.0f)
        };
        return projection;
    }

    Vector3 Math::ForwardFromRotation(const Quaternion& rotation)
    {
        return rotation * Vector3::Forward;
    }

    Vector3 Math::UpFromRotation(const Quaternion& rotation)
    {
        return rotation * Vector3::Up;
    }

    Vector3 Math::RightFromRotation(const Quaternion& rotation)
    {
        return rotation * Vector3::Right;
    }

    Matrix4 Math::LocalToWorldNormal(const Vector3& translation, const Vector3& eulerAnglesDegrees)
    {
        Matrix4 result = Matrix4::Identity;
        result.RotateDegrees(eulerAnglesDegrees);
        result = result.Inverted();
        return result;
    }
}
