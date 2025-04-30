#include "D3D12Shader.h"
#include "Runtime/Application.h"
#include "D3D12Renderer.h"
#include "Runtime/Log.h"
#include <slang/slang.h>


namespace Nova
{
    static slang::IBlob* DiagnosticsBlob = nullptr;
    static StringView GetDiagnostic(slang::IBlob* InDiagnostic)
    {
        if (!InDiagnostic)
            return nullptr;

        return StringView((const char*)InDiagnostic->getBufferPointer(), InDiagnostic->getBufferSize());
    }

    D3D12Shader::D3D12Shader(Renderer* Renderer, const String& Name, const Path& Filepath) : Shader(Renderer, Name, Filepath)
    {
        slang::IGlobalSession* Slang = m_Renderer->GetOwner()->GetSlangSession();
        slang::TargetDesc TargetDesc = {};
        TargetDesc.format = SLANG_SPIRV;
        TargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;

        slang::SessionDesc SessionDesc = {};
        SessionDesc.targetCount = 1;
        SessionDesc.targets = &TargetDesc;
        if (SLANG_FAILED(Slang->createSession(SessionDesc, &m_Compiler)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create Slang session!");
        }
    }

    bool D3D12Shader::Compile()
    {
        if (!m_Compiler)
            return false;

        m_ShaderModule = m_Compiler->loadModuleFromSource(*m_Name, m_Filepath.string().c_str(), nullptr, &DiagnosticsBlob);
        if (!m_ShaderModule)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to compile shader: Failed to load module");
            NOVA_LOG(Shader, Verbosity::Error, "{}", GetDiagnostic(DiagnosticsBlob));
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

    bool D3D12Shader::Link()
    {
        if (!m_Compiler) return false;

        Array<slang::IComponentType*> EntryPoints;
        for (const D3D12ShaderModule& Module : m_ShaderModules)
            EntryPoints.Add(Module.EntryPoint);

        if (SLANG_FAILED(m_Compiler->createCompositeComponentType(EntryPoints.Data(), EntryPoints.Count(), &m_Program, &DiagnosticsBlob)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create program!");
            NOVA_LOG(Shader, Verbosity::Error, "{}", GetDiagnostic(DiagnosticsBlob));
            return false;
        }


        if (SLANG_FAILED(m_Program->link(&m_LinkedProgram, &DiagnosticsBlob)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to link program!");
            NOVA_LOG(Shader, Verbosity::Error, "{}", GetDiagnostic(DiagnosticsBlob));
            return false;
        }
        return true;
    }

    bool D3D12Shader::Validate()
    {
        if (!m_LinkedProgram) return false;

        for (size_t i = 0; i < m_ShaderModules.Count(); i++)
        {
            const SlangResult Result = m_LinkedProgram->getEntryPointCode(i, 0, &m_ShaderModules[i].CompiledCode, &DiagnosticsBlob);
            if (SLANG_FAILED(Result))
            {
                NOVA_LOG(Application, Verbosity::Error, "Failed to get entry point code!");
                NOVA_LOG(Shader, Verbosity::Error, "{}", GetDiagnostic(DiagnosticsBlob));
                return false;
            }
        }
        return true;
    }

    bool D3D12Shader::Bind()
    {
        return true;
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

    bool D3D12Shader::FindShaderStage(const StringView& Name, ShaderStage Stage)
    {
        slang::IEntryPoint* EntryPoint = nullptr;
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint(*Name, ConvertShaderStage(Stage), &EntryPoint, nullptr)))
            return false;

        if (EntryPoint)
        {
            D3D12ShaderModule Module;
            Module.Stage = Stage;
            Module.EntryPoint = EntryPoint;
            m_ShaderModules.Add(Module);
            NOVA_LOG(Application, Verbosity::Info, "Shader {}: Found entry point {}.", GetObjectName(), Name);
        }
        return true;
    }

    void D3D12Shader::Delete()
    {
    }

    const D3D12ShaderModule* D3D12Shader::GetShaderModule(ShaderStage Stage) const
    {
        for (const D3D12ShaderModule& Module : m_ShaderModules)
        {
            if (Module.Stage == Stage)
                return &Module;
        }
        return nullptr;
    }

    const D3D12ShaderModule* D3D12Shader::GetVertexShaderModule() const
    {
        return GetShaderModule(ShaderStage::Vertex);
    }

    const D3D12ShaderModule* D3D12Shader::GetFragmentShaderModule() const
    {
        return GetShaderModule(ShaderStage::Fragment);
    }
}
