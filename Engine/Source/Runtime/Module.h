#pragma once
#include "Containers/String.h"

namespace Nova
{
    class ModuleManager;

    enum class ModuleLoadPhase
    {
        OnEnginePreInit,
        OnEngineInit,
        OnEnginePostInit,
    };
    
    class Module
    {
    public:
        virtual void OnCreate(ModuleManager* Manager) = 0;
        virtual void OnDestroy() = 0;
        virtual ModuleLoadPhase GetModuleLoadPhase() = 0;

        String GetName() const { return m_Name;}
    private:
        friend class ModuleManager;
        Module(String Name) : m_Name(std::move(Name)) {}
        virtual ~Module() = default;

        String m_Name;
    };
}
