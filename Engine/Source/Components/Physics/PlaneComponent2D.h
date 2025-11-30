#pragma once
#include "PhysicsComponent.h"

namespace Nova
{
    struct Vector2;

    class PlaneComponent2D : public PhysicsComponent
    {
    public:
        explicit PlaneComponent2D(Entity* owner) : PhysicsComponent(owner, "Plane Component 2D"){}
        ~PlaneComponent2D() override {}

        void OnInit() override;
        void OnDrawDebug() override;
        void OnDestroy() override;
        void OnGui() override;

        void SetShapePosition(const Vector2& position);
        void SetShapeRotation(float rotation);
        Vector2 GetShapePosition() const;
        float GetShapeRotation();

        float GetWidth() const;
        void SetWidth(float width);
    };
}
