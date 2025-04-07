#pragma once
#include "Runtime/Asset.h"
#include "InputAction.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API InputActionMap : public Asset
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
}
