#include "LinearAlgebra.h"

namespace Nova
{
    Matrix2 Math::Rotate(const Matrix2& Mat, f32 Radians)
    {
        const Matrix2 RotationMatrix = { { Cos(Radians), Sin(Radians) }, { -Sin(Radians), Cos(Radians) } };
        return Mat * RotationMatrix;
    }

    Matrix2 Math::RotateDegrees(const Matrix2& Mat, f32 Degrees)
    {
        return Rotate(Mat, Radians(Degrees));
    }
    
    Matrix2 Math::Scale(const Matrix2& Mat, const Vector2& Scale)
    {
        const Matrix2 ScaleMatrix { { Scale.x, 0.0f }, { 0.0f, Scale.x } };
        return Mat * ScaleMatrix;
    }

    Matrix2 Math::Scale(const Matrix2& Mat, f32 Scale)
    {
        const Matrix2 ScaleMatrix { { Scale, 0.0f }, { 0.0f, Scale }};
        return Mat * ScaleMatrix;
    }

    Matrix3 Math::Rotate(const Matrix3& Mat, const Vector3& Axis, f32 Radians)
    {
        const f32 C = Cos(Radians);
        const f32 S = Sin(Radians);
        
        const Matrix3 XRotationMatrix = {
            { 1, 0, 0 },
            { 0, C, S },
            { 0, -S, C },
        };
        
        const Matrix3 YRotationMatrix = {
            {  C, 0, -S },
            {  0, 1,  0 },
            {  S, 0,  C },
        };

        const Matrix3 ZRotationMatrix = {
            {  C, S, 0 },
            { -S, C, 0 },
            {  0, 0, 1 },
        };

        const Matrix3& RotMat =
            Axis == Vector3::Right ? XRotationMatrix :
            Axis == Vector3::Up ? YRotationMatrix :
            Axis == Vector3::Forward ? ZRotationMatrix : Matrix3::Identity;

        return RotMat * Mat;
    }

    Matrix3 Math::RotateDegrees(const Matrix3& Mat, const Vector3& Axis, f32 Degrees)
    {
        return Rotate(Mat, Axis, Radians(Degrees));
    }
    
    Matrix3 Math::Scale(const Matrix3& Mat, const Vector3& Scale)
    {
        const Matrix3 ScaleMatrix = {
            {Scale.x, 0.0f, 0.0f},
            {0.0f, Scale.y, 0.0f},
            {0.0f, 0.0f, Scale.z},
        };
        return Mat * ScaleMatrix;
    }

    Matrix3 Math::Scale(const Matrix3& Mat, f32 Scale)
    {
        const Matrix3 ScaleMatrix = {
            {Scale, 0.0f, 0.0f},
            {0.0f, Scale, 0.0f},
            {0.0f, 0.0f, Scale},
        };
        return Mat * ScaleMatrix;
    }

#define USE_COMBINED_ROTATION_MATRIX 1

    Matrix4 Math::RotateAxisAngle(const Matrix4& Mat, const Vector3& Axis, f32 AngleRadians)
    {
        const f32 C = Cos(AngleRadians);
        const f32 S = Sin(AngleRadians);
#if USE_COMBINED_ROTATION_MATRIX
        const f32 InvC = 1.0f - C;
        const Vector3 A = Math::Normalize(Axis);

        const Vector4 Column0
        {
            A.x * A.x * InvC + C,
            A.x * A.y * InvC + A.z * S,
            A.x * A.z * InvC - A.y * S,
            0
        };

        const Vector4 Column1
        {
            A.x * A.y * InvC - A.z * S,
            A.y * A.y * InvC + C,
            A.y * A.z * InvC + A.x * S,
            0
        };

        const Vector4 Column2
        {
            A.x * A.z * InvC + A.y * S,
            A.y * A.z * InvC - A.x * S,
            A.z * A.z * InvC + C,
            0
        };

        const Vector4 Column3
        {
            0, 0, 0, 1
        };

        const Matrix4 RotationMatrix = { Column0, Column1, Column2, Column3 };
        return Mat * RotationMatrix;
#else
        const Matrix4 XRotationMatrix = {
            { 1, 0, 0, 0 },
            { 0, C, S, 0 },
            { 0, -S, C,0 },
            { 0, 0, 0, 1 },
        };
        
        const Matrix4 YRotationMatrix = {
            {  C, 0, -S, 0 },
            {  0, 1,  0, 0 },
            {  S, 0,  C, 0 },
            {  0, 0,  0, 1 }
        };

        const Matrix4 ZRotationMatrix = {
            {  C, S, 0, 0 },
            { -S, C, 0, 0 },
            {  0, 0, 1, 0 },
            {  0, 0, 0, 1 }
        };
        
        const Matrix4& RotMat =
            Axis == Vector3::Right ? XRotationMatrix :
            Axis == Vector3::Up ? YRotationMatrix :
            Axis == Vector3::Forward ? ZRotationMatrix : Matrix4::Identity;
        
        return RotMat * Mat;
#endif
    }

    Matrix4 Math::RotateAxisAngleDegrees(const Matrix4& Mat, const Vector3& Axis, f32 AngleDegrees)
    {
        return RotateAxisAngle(Mat, Axis, Radians(AngleDegrees));
    }

    Matrix4 Math::Scale(const Matrix4& Mat, const Vector3& Scale)
    {
        const Matrix4 ScaleMatrix = {
            {Scale.x, 0.0f, 0.0, 0.0f},
            {0.0f, Scale.y, 0.0f, 0.0f},
            {0.0f, 0.0f, Scale.z, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
        return Mat * ScaleMatrix;
    }

    Matrix4 Math::Scale(const Matrix4& Mat, f32 Scale)
    {
        const Matrix4 ScaleMatrix = {
            {Scale, 0.0f, 0.0, 0.0f},
            {0.0f, Scale, 0.0f, 0.0f},
            {0.0f, 0.0f, Scale, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f},
        };
        return Mat * ScaleMatrix;
    }

    Matrix4 Math::RotateEulerAngles(const Matrix4& Mat, const Vector3& EulerAnglesRadians)
    {
        Matrix4 Result = Matrix4::Identity;
        Result.Rotate(Vector3::Forward, EulerAnglesRadians.z);
        Result.Rotate(Vector3::Up, EulerAnglesRadians.y);
        Result.Rotate(Vector3::Right, EulerAnglesRadians.x); 
        return Mat * Result;
    }

    Matrix4 Math::RotateEulerAnglesDegrees(const Matrix4& Mat, const Vector3& EulerAnglesDegrees)
    {
        return RotateEulerAngles(Mat, EulerAnglesDegrees.Apply(Radians));
    }
    
    Matrix4 Math::RotateEulerAnglesAroundPoint(const Matrix4& Mat, const Vector3& EulerAnglesRadians, const Vector3& Point)
    {
        const Matrix4 TranslationIn = Translate(Matrix4::Identity, -Point);
        const Matrix4 Rotation = RotateEulerAngles(Matrix4::Identity, EulerAnglesRadians);
        const Matrix4 TranslationOut = Translate(Matrix4::Identity, Point);
        return TranslationOut * Rotation * TranslationIn * Mat;
    }

    Matrix4 Math::RotateEulerAnglesAroundPointDegrees(const Matrix4& Mat, const Vector3& EulerAnglesDegrees, const Vector3& Point)
    {
        return RotateEulerAnglesAroundPoint(Mat, EulerAnglesDegrees.Apply(Radians), Point);
    }

    Matrix4 Math::RotateAxisAngleAroundPoint(const Matrix4& Mat, const Vector3& Axis, f32 AngleRadians, const Vector3& Point)
    {
        const Matrix4 TranslationIn = Translate(Matrix4::Identity, -Point);
        const Matrix4 Rotation = RotateAxisAngle(Matrix4::Identity, Axis, AngleRadians);
        const Matrix4 TranslationOut = Translate(Matrix4::Identity, Point);
        return TranslationOut * Rotation * TranslationIn * Mat;
    }

    Matrix4 Math::RotateAxisAngleAroundPointDegrees(const Matrix4& Mat, const Vector3& Axis, f32 AngleDegrees, const Vector3& Point)
    {
        return RotateAxisAngleAroundPoint(Mat, Axis, Radians(AngleDegrees), Point);
    }
    
    Matrix4 Math::LookAt(const Vector3& Eye, const Vector3& Center, const Vector3& Up)
    {
        Matrix4 Result;

        Vector3 Z = Eye - Center;
        Z = Normalize(Z);
        Vector3 Y = Up;
        Vector3 X = Y.Cross(Z);
        Y = Z.Cross(X);
        X = Normalize(X);
        Y = Normalize(Y);

        Result[0][0] = X.x;
        Result[1][0] = X.y;
        Result[2][0] = X.z;
        Result[3][0] = -X.Dot(Eye);
        Result[0][1] = Y.x;
        Result[1][1] = Y.y;
        Result[2][1] = Y.z;
        Result[3][1] = -Y.Dot(Eye);
        Result[0][2] = Z.x;
        Result[1][2] = Z.y;
        Result[2][2] = Z.z;
        Result[3][2] = -Z.Dot(Eye);
        Result[0][3] = 0;
        Result[1][3] = 0;
        Result[2][3] = 0;
        Result[3][3] = 1.0f;
        return Result;
    }

    

    Matrix4 Math::Translate(const Matrix4& Mat, const Vector3& Translation)
    {
        const Vector4 Trans(Translation, 1.0f);
        Matrix4 Result(Mat);
        Result[3] = Trans;
        return Result;
    }

    Matrix4 Math::Perspective(f32 FOV, f32 AspectRatio, f32 Near, f32 Far)
    {
        const f32 m22 = -((Far + Near) / (Far - Near));
        const f32 m23 = -((2.0f * Far * Near) / (Far - Near));
        const f32 Scale = 1.0f / Tan(Radians(FOV / 2.0f));
        return {
                  {1.0f / AspectRatio * Scale, 0.0f, 0.0f, 0.0f},
                  {0.0f, Scale, 0.0f, 0.0f},
                  {0.0f, 0.0f, m22,   -1.0f},
                  {0.0f, 0.0f, m23, 0.0f},
            };
    }
    
    Matrix4 Math::Orthographic(f32 Width, f32 Height, f32 Scale, f32 Near, f32 Far)
    {
        const f32 x = Scale * 2.0f / Width;
        const f32 y = Scale * 2.0f /  Height;
        const f32 z = -2.0f / (Far - Near);
        const f32 z2 = -((Far + Near) / (Far - Near));
        return {
            {x   , 0.0f, +0.0f, +0.0f},
            {0.0f, y   , +0.0f, +0.0f},
            {0.0f, 0.0f, +z   , +0.0f},
            {0.0f, 0.0f, z2   , +1.0f},
        };
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
        Result = Result.Inverse();
        Result.Translate(Translation);
        return Result;
    }
}
