#pragma once
#include "Components/Physics/PhysicsComponent.h"
#include "Editor/Components/ComponentEditor.h"

namespace Nova
{
    class PhysicsComponentEditor : public Editor<PhysicsComponent>
    {
    public:
        explicit PhysicsComponentEditor(const Ref<PhysicsComponent>& target) : Editor(target){}
        void OnInspectorGui() override;
    };
}
