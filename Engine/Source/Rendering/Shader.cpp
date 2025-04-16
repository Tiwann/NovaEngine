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

    const ShaderSource& Shader::GetSource() const
    {
        return m_Source;
    }

    Shader::Shader(Renderer* Renderer, const String& Name, Path Filepath)
        : Asset(Name), m_Renderer(Renderer), m_Filepath(std::move(Filepath))
    {
        const Path AbsolutePath = weakly_canonical(m_Filepath);
        const String& ShaderName = GetName();

        NOVA_LOG(Shader, Verbosity::Trace, "Loading shader source: {}", ShaderName);
        if(!File::Exists(AbsolutePath))
        {
            NOVA_LOG(Shader, Verbosity::Error, "Cannot load shader: File doesn't exist!");
            NOVA_LOG(Shader, Verbosity::Error, "File: {}", m_Filepath.string());
            return;
        }

        const String FileContent = File::ReadToString(AbsolutePath);
        SplitSources(FileContent);

        NOVA_LOG(Shader, Verbosity::Trace, "Preprocessing shader \"{}\"", ShaderName);
        if(!Preprocess(m_Source.Vertex) || !Preprocess(m_Source.Fragment))
        {
            NOVA_LOG(Shader, Verbosity::Info, "Shader preprocess failed. File {}.", ShaderName);
            return;
        }

        NOVA_LOG(Shader, Verbosity::Info, "Shader successfully loaded shader source from: {}", ShaderName);
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
        
        const String FileContent = File::ReadToString(AbsolutePath);
        SplitSources(FileContent);

        //NOVA_LOG(Shader, Verbosity::Trace, "Preprocessing shader \"{}\"", ShaderName);
        //if(!Preprocess(m_Source.Vertex) || !Preprocess(m_Source.Fragment))
        //{
        //    NOVA_LOG(Shader, Verbosity::Info, "Shader preprocess failed. File {}.", ShaderName);
        //    return;
        //}
        
        NOVA_LOG(Shader, Verbosity::Info, "Shader successfully loaded shader source from: {}", ShaderName);
    }

    void Shader::SplitSources(const String& Source)
    {
        const String PragmaVertex = "#pragma vertex";
        const String PragmaFragment = "#pragma fragment";
        const String::SizeType VertexShaderPosition = Source.Find(PragmaVertex);
        const String::SizeType FragmentShaderPosition = Source.Find(PragmaFragment);
        if (!(VertexShaderPosition != -1ULL || FragmentShaderPosition != -1ULL))
        {
            NOVA_LOG(Shader, Verbosity::Error, "Please use #pragma directives to tell the shader compiler what is the shader type");
            return;
        }
        m_Source.Vertex = Source.Substring(VertexShaderPosition, FragmentShaderPosition - 1 - VertexShaderPosition);
        m_Source.Fragment = Source.Substring(FragmentShaderPosition);
        m_Source.Vertex = m_Source.Vertex.Substring(PragmaVertex.Count() + 1);
        m_Source.Fragment = m_Source.Fragment.Substring(PragmaFragment.Count() + 1);
    }
    

    bool Shader::Preprocess(String& Source)
    {
        BufferView<u8> SourceView = Source.GetView<u8>();
        MemoryStream Stream(SourceView);
        while (Stream.IsGood())
        {
            String Line;
            Stream.ReadLine(Line);
            if (Line.StartsWith("#include"))
            {
                char IncludeBuffer[256];
                Line.ScanFormat<char>(R"(#include "%[^"]")", IncludeBuffer);

                String IncludeStr(IncludeBuffer);
                const Path Directory = File::GetDirectory(m_Filepath);
                const Path IncludePath = File::GetAbsolutePath(Directory, *IncludeStr);
                if(!File::Exists(IncludePath))
                {
                    NOVA_LOG(Shader, Verbosity::Error, "Failed to preprocess shader: File {} doesn't exist!", IncludePath.string());
                    return false;
                }

                String IncludeContent = File::ReadToString(IncludePath).Append('\n');
                const String::SizeType Position = Source.Find(Line);
                Source.Replace(Position, Line.Count(), IncludeContent);

                const Stream::OffsetType StreamPos = (Stream::OffsetType)(Position + IncludeContent.Count());
                SourceView = Source.GetView<u8>();
                Stream = MemoryStream(SourceView);
                Stream.Seek(Seek::Begin, StreamPos);
                continue;
            }

            if(Line.StartsWith("#define"))
            {
                StaticArray<char, 256> KeyBuffer;
                StaticArray<char, 256> ValueBuffer;
                if (Line.ScanFormat("#define %s %[^\n]", KeyBuffer.Data(), ValueBuffer.Data()) == 2)
                {
                    ShaderSource::Define Define;
                    Define.Key = String(KeyBuffer.Data());
                    Define.Value = String(ValueBuffer.Data());
                    m_Source.Defines.Add(Define);
                    continue;
                }
                
                if (Line.ScanFormat("#define %[^\n]", KeyBuffer.Data()) == 1)
                {
                    ShaderSource::Define Define;
                    Define.Key = String(KeyBuffer.Data());
                    m_Source.Defines.Add(Define);
                    continue;
                }
            }

            
        }
        
        for (const ShaderSource::Define& Define : m_Source.Defines)
        {
            m_Source.OnSources([&Define](String& Source)
            {
                if (Define.Value.has_value())
                {
                    Source.ReplaceAll(Define.Key, Define.Value.value());
                }
            });
        }
        return true;
    }
}