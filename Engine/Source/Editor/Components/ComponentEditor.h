#pragma once
#include "Editor/Editor.h"
#include "Runtime/Component.h"

namespace Nova
{
    template<typename ComponentType> requires std::is_base_of_v<Component, ComponentType>
    class ComponentEditor : public Editor<ComponentType>
    {
    public:
        explicit ComponentEditor(ComponentType* target) : Editor<ComponentType>(target){}

        void OnGui() override
        {

        }
    };
}