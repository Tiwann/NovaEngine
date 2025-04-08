#pragma once
#include "Containers/Map.h"
#include "Containers/String.h"

namespace Nova
{
    class Module;
    
    class ModuleManager
    {
    public:
        using ModuleMap = Map<String, Module*>;
        void Initialize();
        void Shutdown();

        template <typename ModuleType> requires IsBaseOfValue<Module, ModuleType>
        Module* CreateModule()
        {
            ModuleType* Module = new ModuleType();
            m_Modules[Module->m_Name] = Module;
            return Module;
        }
        
        Module* GetModule(const String& Name);

        template <typename ModuleType> requires IsBaseOfValue<Module, ModuleType>
        ModuleType* GetModule(const String& Name)
        {
            return dynamic_cast<ModuleType*>(GetModule(Name));
        }

    private:
        Map<String, Module*> m_Modules;
    };
}
