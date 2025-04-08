#include "ShaderManager.h"
#include "Core/LogVerbosity.h"
#include "Core/Log.h"
#include "Core/Containers/StringFormat.h"
#include "Core/Application.h"

namespace Nova
{
    Shader* ShaderManager::Load(const String& Name, const Path& Filepath)
    {
        Shader* LoadedShader = Shader::Create(*Name, Filepath);
        if(!LoadedShader) return nullptr;
        
        if(!LoadedShader->Compile())
        {
            delete LoadedShader;
            g_Application->RequireExit();
            return nullptr;
        }
        
        if(!LoadedShader->Link())
        {
            delete LoadedShader;
            return nullptr;
        }

        if(!LoadedShader->Validate())
        {
            delete LoadedShader;
            return nullptr;
        }
        
        m_Data[Name] = LoadedShader;
        return LoadedShader;
    }

    Shader* ShaderManager::Retrieve(const String& Name)
    {
        if(!m_Data.Contains(Name))
        {
            NOVA_LOG(ShaderManager, Verbosity::Warning, "Shader \"{}\" not found!", Name);
            return nullptr;
        }
        return m_Data[Name];
    }

    void ShaderManager::ReloadAll()
    {
        for(auto& [Name, Shader] : m_Data)
        {
            Shader->Reload();
        }
    }
}
