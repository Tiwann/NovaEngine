#include "Shader.h"
#include "Renderer.h"
#include "Runtime/Application.h"
#include "Runtime/Assertion.h"
#include "Runtime/Log.h"
#include "Runtime/LogVerbosity.h"
#include "Platform/PlatformShader.h"
#include "Components/Rendering/DirectionalLight.h"
#include "Serialization/MemoryStream.h"


namespace Nova
{
    String Shader::GetAssetType() const
    {
        return "Shader";
    }

    void Shader::SetLight(const String& Name, const LightComponent* Light)
    {
        if (const DirectionalLight* DirLight = dynamic_cast<const DirectionalLight*>(Light))
        {
            SetDirectionalLight(Name, DirLight);
            return;
        }

        if (const PointLight* DirLight = dynamic_cast<const PointLight*>(Light))
        {
            SetPointLight(Name, DirLight);
            return;
        }
        
        if (const AmbientLight* AmbLight = dynamic_cast<const AmbientLight*>(Light))
        {
            SetAmbientLight(Name, AmbLight);
            return;
        }
        
    }

    String Shader::GetFilename() const
    {
        return m_Filepath.stem().string().c_str();
    }

    Shader::Shader(Renderer* Renderer, const String& Name, Path Filepath)
        : Asset(Name), m_Renderer(Renderer), m_Filepath(std::move(Filepath))
    {
        const Path AbsolutePath = weakly_canonical(m_Filepath);
        const String& ShaderName = GetObjectName();

        NOVA_LOG(Shader, Verbosity::Trace, "Loading shader source: {}", ShaderName);
        if(!File::Exists(AbsolutePath))
        {
            NOVA_LOG(Shader, Verbosity::Error, "Cannot load shader: File doesn't exist!");
            NOVA_LOG(Shader, Verbosity::Error, "File: {}", m_Filepath.string());
            return;
        }

        m_Source = File::ReadToString(AbsolutePath);
    }
    
    void Shader::LoadSource(Path Filepath)
    {
        m_Filepath = std::move(Filepath);

        const Path AbsolutePath = weakly_canonical(m_Filepath);
        const String& ShaderName = m_Name;
        NOVA_LOG(Shader, Verbosity::Trace, "Loading shader source: {}", ShaderName);
        
        if(!File::Exists(AbsolutePath))
        {
            NOVA_LOG(Shader, Verbosity::Error, "Cannot load shader: File doesn't exist!");
            NOVA_LOG(Shader, Verbosity::Error, "File: {}", m_Filepath.string());
            return;
        }
        
        m_Source = File::ReadToString(AbsolutePath);
    }
}