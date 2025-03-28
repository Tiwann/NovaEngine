#pragma once
#include "RaycastHit.h"
#include <box2d/b2_world_callbacks.h>


namespace Nova
{
    class Raycast2DCallback : public b2RayCastCallback
    {
    public:
        RaycastHit2D HitInfo;
        f32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, f32 fraction) override;
    };
}
