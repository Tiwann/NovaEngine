#pragma once
#include "InputActionBinding.h"
#include <CoreExport.h>

namespace Nova
{
    class CORE_API InputAction : public Asset
    {
    public:
        String GetAssetType() const override;

        void SetActionName(const String& Name) { m_ActionName = Name; }
        const String& GetActionName() const { return m_ActionName; }


    private:
        String m_ActionName;
        Array<InputActionBinding*> m_Bindings;
    };
}