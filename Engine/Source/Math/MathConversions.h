#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <CoreExport.h>

#include <box2d/b2_math.h>

#include <Jolt/Jolt.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Vec4.h>

#include <imgui.h>

///////////////////////////////////////////////////////////////
/// Box2D
///////////////////////////////////////////////////////////////
inline CORE_HIDDEN b2Vec2 ToB2Vec2(const Nova::Vector2& Vec)
{
    return b2Vec2(Vec.x, Vec.y);
}

inline CORE_HIDDEN b2Vec3 ToB2Vec3(const Nova::Vector3& Vec)
{
    return b2Vec3(Vec.x, Vec.y, Vec.z);
}

inline CORE_HIDDEN Nova::Vector2 ToVector2(const b2Vec2& Vec)
{
    return Nova::Vector2(Vec.x, Vec.y);
}

inline CORE_HIDDEN Nova::Vector3 ToVector3(const b2Vec3& Vec)
{
    return Nova::Vector3(Vec.x, Vec.y, Vec.z);
}


///////////////////////////////////////////////////////////////
/// JOLT
///////////////////////////////////////////////////////////////
inline CORE_HIDDEN JPH::Vec3 ToJoltVec3(const Nova::Vector3& Vec)
{
    return JPH::Vec3(Vec.x, Vec.y, Vec.z);
}

inline CORE_HIDDEN JPH::Vec4 ToJoltVec4(const Nova::Vector4& Vec)
{
    return JPH::Vec4(Vec.x, Vec.y, Vec.z, Vec.w);
}

inline CORE_HIDDEN Nova::Vector3 ToVector3(const JPH::Vec3& Vec)
{
    return Nova::Vector3(Vec.GetX(), Vec.GetY(), Vec.GetZ());
}

inline CORE_HIDDEN Nova::Vector4 ToVector4(const JPH::Vec4& Vec)
{
    return Nova::Vector4(Vec.GetX(), Vec.GetY(), Vec.GetZ(), Vec.GetW());
}

///////////////////////////////////////////////////////////////
/// ImGui
///////////////////////////////////////////////////////////////

inline CORE_HIDDEN Nova::Vector2 ToVector2(const ImVec2& Vec)
{
    return Nova::Vector2(Vec.x, Vec.y);
}

inline CORE_HIDDEN Nova::Vector4 ToVector4(const ImVec4& Vec)
{
    return Nova::Vector4(Vec.x, Vec.y, Vec.z, Vec.w);
}

inline CORE_HIDDEN ImVec2 ToImVec2(const Nova::Vector2& Vec)
{
    return ImVec2(Vec.x, Vec.y);
}

inline CORE_HIDDEN ImVec4 ToImVec4(const Nova::Vector4& Vec)
{
    return ImVec4(Vec.x, Vec.y, Vec.z, Vec.w);
}