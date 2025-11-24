#pragma once
#include "Components/Physics/PhysicsComponent.h"
#include "Editor/Components/ComponentEditor.h"

namespace Nova
{
    class PhysicsComponentEditor : public Editor<PhysicsComponent>
    {
    public:
        explicit PhysicsComponentEditor(PhysicsComponent* target) : Editor(target){}
        void OnInspectorGui() override;
    };
}
