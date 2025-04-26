#include "OpenGLShader.h"
#include "Runtime/Assertion.h"
#include "Runtime/Log.h"
#include "Runtime/LogVerbosity.h"
#include "Runtime/Entity.h"
#include "Rendering/Texture2D.h"
#include "Components/Transform.h"
#include "Components/Rendering/DirectionalLight.h"
#include "Containers/StringFormat.h"
#include "Runtime/Application.h"

#include <glad/gl.h>
#include <slang/slang.h>

#include "OpenGLRenderer.h"


namespace Nova
{
    OpenGLShader::OpenGLShader(Renderer* Renderer, const String& Name, Path Filepath)
        : Shader(Renderer, Name, std::move(Filepath))
    {

        Array<slang::CompilerOptionEntry> CompileOptions;


        slang::IGlobalSession* Slang = m_Renderer->GetOwner()->GetSlangSession();
        slang::TargetDesc TargetDesc = {};
        TargetDesc.format = SLANG_SPIRV;
        TargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;
        TargetDesc.compilerOptionEntries = CompileOptions.Data();
        TargetDesc.compilerOptionEntryCount = CompileOptions.Count();

        slang::SessionDesc SessionDesc = {};
        SessionDesc.targetCount = 1;
        SessionDesc.targets = &TargetDesc;

        if (SLANG_FAILED(Slang->createSession(SessionDesc, &m_Session)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create Slang session!");
        }
    }
    
    OpenGLShader::~OpenGLShader()
    {
        for (const OpenGLShaderModule& Module : m_ShaderModules)
        {
            glDeleteShader(Module.Handle);
        }
        glDeleteProgram(m_ProgramHandle);
    }



    bool OpenGLShader::Compile()
    {
        if (!m_Session)
            return false;

        m_Module = m_Session->loadModuleFromSource(*m_Name, m_Filepath.string().c_str(), nullptr, nullptr);
        if (!m_Module)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to compile shader: Failed to load module");
            return false;
        }

        FindShaderStage("VertexMain", ShaderStage::Vertex);
        FindShaderStage("GeometryMain", ShaderStage::Geometry);
        FindShaderStage("FragmentMain", ShaderStage::Fragment);

        if (m_ShaderModules.IsEmpty())
        {
            NOVA_LOG(Shader, Verbosity::Error, "No shader stage found!");
            return false;
        }
        return true;
    }

    bool OpenGLShader::Link()
    {
        if (!m_Session) return false;

        Array<slang::IComponentType*> EntryPoints;
        for (const OpenGLShaderModule& Module : m_ShaderModules)
            EntryPoints.Add(Module.EntryPoint);

        if (SLANG_FAILED(m_Session->createCompositeComponentType(EntryPoints.Data(), EntryPoints.Count(), &m_Program, nullptr)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create program!");
            return false;
        }

        if (SLANG_FAILED(m_Program->link(&m_LinkedProgram, nullptr)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to link program!");
            return false;
        }
        return true;
    }

    bool OpenGLShader::Validate()
    {
        if (!m_LinkedProgram) return false;

        const OpenGLRenderer* Renderer = m_Renderer->As<OpenGLRenderer>();
        const OpenGLRendererTypeConvertor& Convertor = Renderer->Convertor;
        for (size_t i = 0; i < m_ShaderModules.Count(); i++)
        {
            if (SLANG_FAILED(m_LinkedProgram->getEntryPointCode(i, 0, &m_ShaderModules[i].CompiledCode)))
            {
                NOVA_LOG(Application, Verbosity::Error, "Failed to get entry point code!");
                return false;
            }
        }

        m_ProgramHandle = glCreateProgram();
        for (OpenGLShaderModule& Module : m_ShaderModules)
        {
            Module.Handle = glCreateShader(Convertor.ConvertShaderStage(Module.Stage));
            glShaderBinary(1, &Module.Handle, GL_SHADER_BINARY_FORMAT_SPIR_V, Module.CompiledCode->getBufferPointer(), Module.CompiledCode->getBufferSize());
            glSpecializeShader(Module.Handle, "main", 0, nullptr, nullptr);
            glAttachShader(m_ProgramHandle, Module.Handle);
        }

        glLinkProgram(m_ProgramHandle);
        i32 Success = 0;

        glGetProgramiv(m_ProgramHandle, GL_LINK_STATUS, &Success);
        if(!Success)
        {
            i32 Length = 0;
            char Message[GL_INFO_LOG_LENGTH];
            glGetProgramInfoLog(m_ProgramHandle, GL_INFO_LOG_LENGTH, &Length, Message);
            NOVA_LOG(Shader, Verbosity::Error, "Shader program failed to link: {}", Message);
        }

        NOVA_LOG(Shader, Verbosity::Info, "Shader program {} successfully linked!", m_Name);
        glValidateProgram(m_ProgramHandle);

        return true;
    }

    bool OpenGLShader::Bind()
    {
        glUseProgram(m_ProgramHandle);
        return true;
    }

    void OpenGLShader::Delete()
    {
        glDeleteProgram(m_ProgramHandle);
    }

    static SlangStage ConvertShaderStage(ShaderStage Stage)
    {
        switch (Stage) {
        case ShaderStage::None: return SLANG_STAGE_FRAGMENT;
        case ShaderStage::Vertex: return SLANG_STAGE_VERTEX;
        case ShaderStage::Geometry: return SLANG_STAGE_GEOMETRY;
        case ShaderStage::Fragment: return SLANG_STAGE_FRAGMENT;
        case ShaderStage::Compute: return SLANG_STAGE_COMPUTE;
        case ShaderStage::RayGeneration: return SLANG_STAGE_RAY_GENERATION;
        case ShaderStage::Tessellation: return SLANG_STAGE_HULL;
        case ShaderStage::Mesh: return SLANG_STAGE_MESH;
        default: throw;
        }
    }

    bool OpenGLShader::FindShaderStage(const StringView& Name, ShaderStage Stage)
    {
        slang::IEntryPoint* EntryPoint = nullptr;
        if (SLANG_FAILED(m_Module->findAndCheckEntryPoint(*Name, ConvertShaderStage(Stage), &EntryPoint, nullptr)))
            return false;

        if (EntryPoint)
        {
            OpenGLShaderModule Module;
            Module.Stage = Stage;
            Module.EntryPoint = EntryPoint;
            m_ShaderModules.Add(Module);

            NOVA_LOG(Application, Verbosity::Info, "Shader {}: Found entry point {}.", GetName(), Name);
        }
        return true;
    }

}
