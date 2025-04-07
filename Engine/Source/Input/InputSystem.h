#pragma once
#include "Runtime/Component.h"
#include "Containers/String.h"
#include "Containers/Array.h"
#include "InputActionMap.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API InputSystem : public Component
    {
    public:
        InputSystem(Entity* Owner) : Component(Owner, "Input System")
        {
            m_Flags = ComponentFlagBits::DisallowMultipleInEntity;
        }

        
        void OnUpdate(float DeltaTime) override;
        void AddActionMap(InputActionMap* ActionMap) { m_ActionMaps.Add(ActionMap); }
        void ClearActionMaps() { m_ActionMaps.Clear(); }
    private:
        Array<InputActionMap*> m_ActionMaps;
    };
}
