#pragma once
#include "PhysicsComponent.h"
#include "Math/Vector2.h"
#include "Physics/PhysicsContactInfo.h"

namespace Nova
{
    class BoxComponent2D final : public PhysicsComponent
    {
    public:
        BoxComponent2D(Entity* owner): PhysicsComponent(owner, "Box Component 2D"){ }

        void OnInit() override;
        void OnDestroy() override;

        void SetShapePosition(const Vector2& position);
        Vector2 GetShapePosition() const;

        void SetShapeRotation(float radians);
        float GetShapeRotation() const;

        void SetWidth(float width);
        float GetWidth() const;
        void SetHeight(float height);
        float GetHeight() const;
        void SetSize(float width, float height);
    private:
        void OnContactBegin(const PhysicsContact& contact);
        void OnContactStay(const PhysicsContact& contact);
        void OnContactEnd(const PhysicsContact& contact);
    };
}
