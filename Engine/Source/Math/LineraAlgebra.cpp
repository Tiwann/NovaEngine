#include "LinearAlgebra.h"

namespace Nova
{
    Matrix2 Math::Rotate(const Matrix2& Mat, const f32 Radians)
    {
        const Matrix2 RotationMatrix
        {
            Vector2(Cos(Radians), Sin(Radians)),
            Vector2(-Sin(Radians), Cos(Radians))
        };
        return RotationMatrix * Mat;
    }

    Matrix2 Math::RotateDegrees(const Matrix2& Mat, const f32 Degrees)
    {
        return Rotate(Mat, Radians(Degrees));
    }
    
    Matrix2 Math::Scale(const Matrix2& Mat, const Vector2& Scale)
    {
        const Matrix2 ScaleMatrix
        {
            Vector2(Scale.x, 0.0f),
            Vector2(0.0f, Scale.y)
        };
        return ScaleMatrix * Mat;
    }

    Matrix2 Math::Scale(const Matrix2& Mat, const f32 Scale)
    {
        const Matrix2 ScaleMatrix
        {
            Vector2(Scale, 0.0f),
            Vector2(0.0f, Scale)
        };
        return ScaleMatrix * Mat;
    }

    Matrix3 Math::RotateAxisAngle(const Matrix3& Mat, const Vector3& Axis, const f32 AngleRadians)
    {
        const Vector3 NormalizedAxis = Normalize(Axis);
        const f32 C = Cos(AngleRadians);
        const f32 S = Sin(AngleRadians);
        const f32 InvC = 1.0f - C;
        const f32 xx = NormalizedAxis.x * NormalizedAxis.x;
        const f32 yy = NormalizedAxis.y * NormalizedAxis.y;
        const f32 zz = NormalizedAxis.z * NormalizedAxis.z;
        const f32 xy = NormalizedAxis.x * NormalizedAxis.y;
        const f32 xz = NormalizedAxis.x * NormalizedAxis.z;
        const f32 yz = NormalizedAxis.y * NormalizedAxis.z;
        const f32 x = NormalizedAxis.x;
        const f32 y = NormalizedAxis.y;
        const f32 z = NormalizedAxis.z;

        const Matrix3 RotationMatrix
        {
            Vector3(xx * InvC + C, xy * InvC + z * S, xz * InvC - y * S),
            Vector3(xy * InvC - z * S, yy * InvC + C, yz * InvC + x * S),
            Vector3(xz * InvC + y * S, yz * InvC - x * S, zz * InvC + C),
        };
        return RotationMatrix * Mat;
    }

    Matrix3 Math::RotateAxisAngleDegrees(const Matrix3& Mat, const Vector3& Axis, const f32 AngleDegrees)
    {
        return RotateAxisAngle(Mat, Axis, Radians(AngleDegrees));
    }

    Matrix3 Math::RotateEulerAngles(const Matrix3& Mat, const Vector3& EulerAnglesRadians)
    {
        Matrix3 Result = Matrix3::Identity;
        Result.Rotate(Vector3::Forward, EulerAnglesRadians.z);
        Result.Rotate(Vector3::Up, EulerAnglesRadians.y);
        Result.Rotate(Vector3::Right, EulerAnglesRadians.x);
        return Result * Mat;
    }

    Matrix3 Math::RotateEulerAnglesDegrees(const Matrix3& Mat, const Vector3& EulerAnglesDegrees)
    {
        return RotateEulerAngles(Mat, EulerAnglesDegrees.Apply(Radians));
    }
    
    Matrix3 Math::Scale(const Matrix3& Mat, const Vector3& Scale)
    {
        const Matrix3 ScaleMatrix
        {
            Vector3(Scale.x, 0.0f, 0.0f),
            Vector3(0.0f, Scale.y, 0.0f),
            Vector3(0.0f, 0.0f, Scale.z),
        };
        return ScaleMatrix * Mat;
    }

    Matrix3 Math::Scale(const Matrix3& Mat, f32 Scale)
    {
        const Matrix3 ScaleMatrix
        {
            Vector3(Scale, 0.0f, 0.0f),
            Vector3(0.0f, Scale, 0.0f),
            Vector3(0.0f, 0.0f, Scale),
        };
        return ScaleMatrix * Mat;
    }

    Matrix3 Math::Translate(const Matrix3& Mat, const Vector2& Translation)
    {
        const f32 tx = Translation.x;
        const f32 ty = Translation.y;

        const Matrix3 TranslationMatrix = {
            Vector3(1.0f, 0.0f, 0.0f),
            Vector3(0.0f, 1.0f, 0.0f),
            Vector3(  tx,   ty, 1.0f),
        };
        return TranslationMatrix * Mat;
    }

    Matrix4 Math::RotateAxisAngle(const Matrix4& Mat, const Vector3& Axis, const f32 AngleRadians)
    {
        const Matrix3 RotationMatrix = RotateAxisAngle(Matrix3::Identity, Axis, AngleRadians);
        const Matrix4 Result
        {
            Vector4(RotationMatrix[0], 0.0f),
            Vector4(RotationMatrix[1], 0.0f),
            Vector4(RotationMatrix[2], 0.0f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f)
        };
        return Result * Mat;
    }

    Matrix4 Math::RotateAxisAngleDegrees(const Matrix4& Mat, const Vector3& Axis, const f32 AngleDegrees)
    {
        return RotateAxisAngle(Mat, Axis, Radians(AngleDegrees));
    }

    Matrix4 Math::Scale(const Matrix4& Mat, const Vector3& Scale)
    {
        const Matrix4 ScaleMatrix
        {
            Vector4(Scale.x, 0.0f, 0.0, 0.0f),
            Vector4(0.0f, Scale.y, 0.0f, 0.0f),
            Vector4(0.0f, 0.0f, Scale.z, 0.0f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f),
        };
        return ScaleMatrix * Mat;
    }

    Matrix4 Math::Scale(const Matrix4& Mat, f32 Scale)
    {
        const Matrix4 ScaleMatrix =
        {
            Vector4(Scale, 0.0f, 0.0, 0.0f),
            Vector4(0.0f, Scale, 0.0f, 0.0f),
            Vector4(0.0f, 0.0f, Scale, 0.0f),
            Vector4(0.0f, 0.0f, 0.0f, 1.0f),
        };
        return ScaleMatrix * Mat;
    }

    Matrix4 Math::RotateEulerAngles(const Matrix4& Mat, const Vector3& EulerAnglesRadians)
    {
        Matrix4 Result = Matrix4::Identity;
        Result.Rotate(Vector3::Forward, EulerAnglesRadians.z);
        Result.Rotate(Vector3::Up, EulerAnglesRadians.y);
        Result.Rotate(Vector3::Right, EulerAnglesRadians.x); 
        return Result * Mat;
    }

    Matrix4 Math::RotateEulerAnglesDegrees(const Matrix4& Mat, const Vector3& EulerAnglesDegrees)
    {
        return RotateEulerAngles(Mat, EulerAnglesDegrees.Apply(Radians));
    }


    Matrix4 Math::Translate(const Matrix4& Mat, const Vector3& Translation)
    {
        Matrix4 TranslationMatrix = Matrix4::Identity;
        TranslationMatrix[3] = Vector4(Translation, 1.0f);
        return TranslationMatrix * Mat;
    }

    Matrix4 Math::Perspective(const f32 FOV, const f32 AspectRatio, const f32 Near, const f32 Far)
    {
        const f32 Scale = 1.0f / Tan(Radians(FOV * 0.5f));
        const f32 m22 = Far / (Near - Far);
        const f32 m32 = -(Far * Near) / (Far - Near);

        return {
            Vector4(Scale / AspectRatio, 0.0f,  0.0f,  0.0f),
            Vector4(0.0f,               Scale,  0.0f,  0.0f),
            Vector4(0.0f,               0.0f,   m22,   -1.0f),
            Vector4(0.0f,               0.0f,   m32,   0.0f)
        };

    }
    
    Matrix4 Math::Orthographic(const f32 Width, const f32 Height, const f32 Scale, const f32 Near, const f32 Far)
    {
        const Matrix4 Projection
        {
            Vector4(Scale * 2.0f / Width, 0.0f, 0.0f, 0.0f),
            Vector4(0.0f, Scale * 2.0f / Height, 0.0f, 0.0f),
            Vector4(0.0f, 0.0f, -1.0f / (Far - Near), 0.0f),
            Vector4(-0.0f, -0.0f, -Near / (Far - Near), 1.0f)
        };
        return Projection;
    }

    Vector3 Math::ForwardFromRotation(const Vector3& EulerAngles)
    {
        Matrix4 Result = Matrix4::Identity;
        Result.RotateDegrees(EulerAngles);
        return Result * Vector3::Forward;
    }

    Vector3 Math::UpFromRotation(const Vector3& EulerAngles)
    {
        Matrix4 Result = Matrix4::Identity;
        Result.RotateDegrees(EulerAngles);
        return Result * Vector3::Up;
    }

    Vector3 Math::RightFromRotation(const Vector3& EulerAngles)
    {
        Matrix4 Result = Matrix4::Identity;
        Result.RotateDegrees(EulerAngles);
        return Result * Vector3::Right;  
    }

    Matrix4 Math::LocalToWorldNormal(const Vector3& Translation, const Vector3& EulerAnglesDegrees)
    {
        Matrix4 Result = Matrix4::Identity;
        Result.RotateDegrees(EulerAnglesDegrees);
        Result = Result.Inverted();
        return Result;
    }
}
