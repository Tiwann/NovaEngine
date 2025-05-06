#include "Math/LinearAlgebra.h"
#include "Runtime/Logger.h"

static Nova::String ToString(const Nova::Vector2& Vec)
{
    return Nova::StringFormat("({:.2f}; {:.2f})", Vec.x, Vec.y);
}

static Nova::String ToString(const Nova::Vector3& Vec)
{
    return Nova::StringFormat("({:.2f}; {:.2f}; {:.2f})", Vec.x, Vec.y, Vec.z);
}

static Nova::String ToString(const Nova::Vector4& Vec)
{
    return Nova::StringFormat("({:.2f}; {:.2f}; {:.2f}; {:.2f})", Vec.x, Vec.y, Vec.z, Vec.w);
}


static Nova::String ToString(const Nova::Quaternion& Quat)
{
    return Nova::StringFormat("({:.2f}; {:.2f}; {:.2f}; {:.2f})", Quat.w, Quat.x, Quat.y, Quat.z);
}


static Nova::Logger Logger("Math Tests");

static void QuaternionTest()
{
    Logger.LogTrace("********** Quaternion Tests **********");
    const Nova::Quaternion Orientation = Nova::Quaternion::EulerDegrees(0.0f, 90.0f, 0.0f);
    const Nova::Quaternion InverseOrientation = Orientation.Inverted();
    const Nova::Vector3 OrientedForward = Orientation * Nova::Vector3::Forward;
    const Nova::Vector3 RevertedVector = InverseOrientation * OrientedForward;
    Logger.LogTrace("Vector3::Forward: {}", ToString(Nova::Vector3::Forward));
    Logger.LogTrace("Orientation: {}", ToString(Orientation));
    Logger.LogTrace("Inverse Orientation: {}", ToString(InverseOrientation));
    Logger.LogTrace("Oriented Vector: {}", ToString(OrientedForward));
    Logger.LogTrace("Reverted Vector: {}", ToString(RevertedVector));
    Logger.LogTrace("Result: {}", OrientedForward == Nova::Vector3(1.0f, 0.0f, 0.0f) && RevertedVector == Nova::Vector3::Forward ? "Pass" : "Fail");
    Logger.LogTrace("**************************************");
}


static void Matrix2Test()
{
    Logger.LogTrace("********** Matrix2 Tests **********");
    Nova::Matrix2 Matrix;
    Nova::Vector2 Rotated = Nova::Vector2::Up * Matrix;
    Logger.LogTrace("**************************************");
}

int main(void)
{
    QuaternionTest();
    return 0;
}