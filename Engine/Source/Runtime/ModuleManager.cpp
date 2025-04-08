#include "ModuleManager.h"
#include "Module.h"

namespace Nova
{
    void ModuleManager::Initialize()
    {
        
    }

    void ModuleManager::Shutdown()
    {
        for (ModuleMap::PairType& Pair : m_Modules)
        {
            Pair.Value->OnDestroy();
            delete Pair.Value;
        }

        m_Modules.Clear();
    }

    Module* ModuleManager::GetModule(const String& Name)
    {
        if (!m_Modules.Contains(Name))
            return nullptr;
        return m_Modules[Name];
    }
}

