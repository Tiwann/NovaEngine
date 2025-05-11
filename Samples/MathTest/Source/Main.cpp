#include "Math/LinearAlgebra.h"
#include "Runtime/Logger.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static Nova::String ToString(const Nova::Vector2& Vec)
{
    return Nova::StringFormat("[{:.2f}; {:.2f}]", Vec.x, Vec.y);
}

static Nova::String ToString(const Nova::Vector3& Vec)
{
    return Nova::StringFormat("[{:.2f}; {:.2f}; {:.2f}]", Vec.x, Vec.y, Vec.z);
}

static Nova::String ToString(const Nova::Vector4& Vec)
{
    return Nova::StringFormat("[{:.2f}; {:.2f}; {:.2f}; {:.2f}]", Vec.x, Vec.y, Vec.z, Vec.w);
}


static Nova::String ToString(const Nova::Quaternion& Quat)
{
    return Nova::StringFormat("[{:.2f}; {:.2f}; {:.2f}; {:.2f}]", Quat.w, Quat.x, Quat.y, Quat.z);
}


static Nova::Logger Logger("Math Tests");

static void QuaternionTest()
{
    Logger.LogTrace("********** Quaternion Tests **********");
    const Nova::Quaternion Orientation = Nova::Quaternion::EulerDegrees(0.0f, 90.0f, 0.0f);
    const Nova::Vector3 OrientedForward = Orientation * Nova::Vector3::Forward;
    const auto MatrixRotation = Nova::Math::RotateAxisAngleDegrees(Nova::Matrix4::Identity, Nova::Vector3::Up, 90.0f);
    const Nova::Vector3 RotatedForward = MatrixRotation * Nova::Vector3::Forward;
    Logger.LogTrace("Vector3::Forward: {}", ToString(Nova::Vector3::Forward));
    Logger.LogTrace("Orientation: {}", ToString(Orientation));
    Logger.LogTrace("Oriented Vector: {}", ToString(OrientedForward));
    Logger.LogTrace("Rotated Vector: {}", ToString(RotatedForward));
}


static void MatrixTests()
{
    {
        Logger.LogTrace("********** Matrix2 Tests **********");
        const Nova::Matrix2 Scale = Nova::Math::Scale(Nova::Math::Identity<Nova::Matrix2>(), Nova::Vector2(2.0f));
        const Nova::Vector2 Result = Scale * Nova::Vector2::One;
        if (Result == Nova::Vector2(2.0f, 2.0f))
        {
            Logger.LogInfo("Matrix2::Scale: Success");
        } else
        {
            Logger.LogError("Matrix2::Scale: Failed");
        }
    }

    {
        const Nova::Matrix2 Rotate = Nova::Math::RotateDegrees(Nova::Math::Identity<Nova::Matrix2>(), 90.0f);
        const Nova::Vector2 Result = Rotate * Nova::Vector2::Right;
        if (Result == Nova::Vector2(0.0f, 1.0f))
        {
            Logger.LogInfo("Matrix2::Rotate: Success");
        } else
        {
            Logger.LogError("Matrix2::Rotate: Failed");
        }
    }


    {
        Logger.LogTrace("********** Matrix3 Tests **********");
        const Nova::Matrix3 Rotate = Nova::Math::RotateAxisAngleDegrees(Nova::Math::Identity<Nova::Matrix3>(), Nova::Vector3::Forward, 90.0f);
        const Nova::Vector3 Result = Rotate * Nova::Vector3::Right;
        if (Result == Nova::Vector3(0.0f, 1.0f, 0.0f))
        {
            Logger.LogInfo("Matrix3::Rotate: Success");
        } else
        {
            Logger.LogError("Matrix3::Rotate: Failed");
        }
    }

    {
        Logger.LogTrace("********** Matrix4 Tests **********");
        const Nova::Matrix4 Rotation4 = Nova::Math::RotateAxisAngleDegrees(Nova::Matrix4::Identity, Nova::Vector3::Up, 90.0f);
        const Nova::Matrix3 Rotation3 = Nova::Math::RotateAxisAngleDegrees(Nova::Matrix3::Identity, Nova::Vector3::Up, 90.0f);
        const Nova::Vector3 Result3 = Rotation3 * Nova::Vector3::Forward;
        const Nova::Vector3 Result4 = Rotation4 * Nova::Vector3::Forward;

        if (Result3 == Result4)
        {
            Logger.LogInfo("Matrix4::Rotate: Success");
        } else
        {
            Logger.LogError("Matrix4::Rotate: Failed");
        }
    }


    {
        Logger.LogTrace("********** Matrix3 Tests (VS GLM)**********");
        const glm::mat4 Rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::vec3 RotatedForward = Rotation * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

        if (RotatedForward == glm::vec3(0.0f, 1.0f, 0.0f))
        {
            Logger.LogInfo("Matrix3::Rotate: Success");
        } else
        {
            Logger.LogError("Matrix3::Rotate: Failed");
        }
    }
}

int main(void)
{
    QuaternionTest();
    MatrixTests();
    return 0;
}