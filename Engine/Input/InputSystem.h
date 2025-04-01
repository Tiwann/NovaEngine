#pragma once
#include "Core/Asset.h"
#include "Core/Component.h"
#include "Core/Containers/String.h"
#include "Core/Containers/DynamicArray.h"

namespace Hydro
{
    class InputActionBinding
    {
        
    };

    
    class InputAction : public Asset
    {
    public:
        String GetAssetType() const override;

        void SetActionName(const String& Name) { m_ActionName = Name; }
        const String& GetActionName() const { return m_ActionName; }

        
    private:
        String m_ActionName;
        Array<InputActionBinding*> m_Bindings;
    };

    class InputActionMap : public Asset
    {
    public:
        String GetAssetType() const override;

        void SetMapName(const String& Name) { m_MapName = Name; }
        const String& GetMapName() const { return m_MapName; }

        void AddAction(InputAction* Action) { m_Actions.Add(Action); }
        void ClearActions() { m_Actions.Clear(); }
    private:
        String m_MapName;
        Array<InputAction*> m_Actions;
    };
    
    class InputSystem : public Component
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
