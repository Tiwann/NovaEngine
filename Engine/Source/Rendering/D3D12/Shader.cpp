#include "Shader.h"

#include "Rendering/ShaderBindingSetLayout.h"
#include "Runtime/Application.h"
#include "Rendering/SlangCommon.h"
#include "Runtime/Path.h"
#include "Runtime/Log.h"

namespace Nova::D3D12
{
    bool Shader::Initialize(const ShaderCreateInfo& createInfo)
    {

        Application& application = Application::GetCurrentApplication();
        slang::IGlobalSession* slangSession = application.GetSlangSession();

        slang::TargetDesc shaderTargetDesc;
        shaderTargetDesc.format = GetCompileTarget(createInfo.target, createInfo.device->GetDeviceType());
        shaderTargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;
        shaderTargetDesc.lineDirectiveMode = SLANG_LINE_DIRECTIVE_MODE_DEFAULT;
        shaderTargetDesc.profile = slangSession->findProfile("sm_5_0");

        slang::CompilerOptionEntry entries[] = {
            {slang::CompilerOptionName::MinimumSlangOptimization, slang::CompilerOptionValue(slang::CompilerOptionValueKind::Int, 1)},
            {slang::CompilerOptionName::Optimization, slang::CompilerOptionValue(slang::CompilerOptionValueKind::Int, SLANG_OPTIMIZATION_LEVEL_MAXIMAL)},
        };

        Array<String> includes;
        includes.Add(Path::GetEngineAssetPath("Shaders/Include"));
        includes.AddRange(createInfo.includes);

        const auto ToConstChar = [](const StringView includeDir) -> const char* { return includeDir; };
        Array<const char*> cstrIncludes = includes.Transform<const char*>(ToConstChar);

        Array<slang::PreprocessorMacroDesc> macros;
        for (size_t i = 0; i < createInfo.defines.Count(); ++i)
        {
            const auto& define = createInfo.defines.GetAt(i);
            slang::PreprocessorMacroDesc desc;
            desc.name = *define.key;
            desc.value = *define.value;
            macros.Add(desc);
        }

        slang::SessionDesc sessionDesc;
        sessionDesc.targets = &shaderTargetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
        sessionDesc.searchPaths = cstrIncludes.Data();
        sessionDesc.searchPathCount = cstrIncludes.Count();
        sessionDesc.compilerOptionEntries = entries;
        sessionDesc.compilerOptionEntryCount = std::size(entries);
        sessionDesc.preprocessorMacros = macros.Data();
        sessionDesc.preprocessorMacroCount = macros.Count();

        SlangResult result = slangSession->createSession(sessionDesc, m_Session.writeRef());
        if (SLANG_FAILED(result)) return false;

        Slang::ComPtr<slang::IBlob> errorBlob = nullptr;

        m_Module = Slang::ComPtr(m_Session->loadModuleFromSource(
            *createInfo.moduleInfo.name,
            *createInfo.moduleInfo.filepath,
            nullptr,
            errorBlob.writeRef()));

        if (!m_Module)
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to load slang module [{}]: {}", *createInfo.moduleInfo.name, *GetErrorString(errorBlob));
            return false;
        }

        for (const ShaderEntryPoint& shaderEntryPoint : createInfo.entryPoints)
        {
            Slang::ComPtr<slang::IEntryPoint> entryPoint = nullptr;
            result = m_Module->findEntryPointByName(*shaderEntryPoint.name, entryPoint.writeRef());
            if (SLANG_FAILED(result))
            {
                NOVA_LOG(RenderDevice, Verbosity::Error, "Entry point '{}' not found. Compilation failed.", *shaderEntryPoint.name);
                return false;
            }
            m_EntryPoints.Add(entryPoint);
        }

        Array<slang::IComponentType*> entryPoints = m_EntryPoints.As<slang::IComponentType*>();

        result = m_Session->createCompositeComponentType(entryPoints.Data(), entryPoints.Count(), m_Program.writeRef(), errorBlob.writeRef());
        if (SLANG_FAILED(result))
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to create shader program: {}", *GetErrorString(errorBlob));
            return false;
        }

        result = m_Program->link(m_LinkedProgram.writeRef(), errorBlob.writeRef());
        if (SLANG_FAILED(result))
        {
            NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to link shader program: {}", *GetErrorString(errorBlob));
            return false;
        }

        for (size_t entryPointIndex = 0; entryPointIndex < m_EntryPoints.Count(); ++entryPointIndex)
        {
            Slang::ComPtr<slang::IBlob> entryPointCode = nullptr;
            result = m_LinkedProgram->getEntryPointCode(entryPointIndex, 0, entryPointCode.writeRef(), errorBlob.writeRef());
            if (SLANG_FAILED(result))
            {
                NOVA_LOG(RenderDevice, Verbosity::Error, "Failed to get entry point code: {}", *GetErrorString(errorBlob));
                return false;
            }

            const ShaderStageFlagBits& shaderStage = createInfo.entryPoints[entryPointIndex].stage;

            ShaderModuleCreateInfo shaderModuleCreateInfo;
            shaderModuleCreateInfo.code = (const uint32_t*)entryPointCode->getBufferPointer();
            shaderModuleCreateInfo.codeSize = entryPointCode->getBufferSize();
            shaderModuleCreateInfo.device = createInfo.device;
            shaderModuleCreateInfo.stage = shaderStage;

            ShaderModule shaderModule;
            shaderModule.Initialize(shaderModuleCreateInfo);
            m_ShaderModules.Add(shaderModule);
        }

        slang::ProgramLayout* programLayout = m_LinkedProgram->getLayout();
        ShaderStageFlags stageFlags = ShaderStageFlagBits::None;
        for (int e = 0; e < programLayout->getEntryPointCount(); ++e)
        {
            slang::EntryPointReflection* ep = programLayout->getEntryPointByIndex(e);
            stageFlags |= GetStage(ep->getStage());
        }

        return true;
    }

    void Shader::Destroy()
    {
        for (auto& shaderModule : m_ShaderModules)
            shaderModule.Destroy();
    }

    Ref<ShaderBindingSet> Shader::CreateBindingSet(size_t setIndex) const
    {
        return nullptr;
    }

    Array<Ref<ShaderBindingSet>> Shader::CreateBindingSets() const
    {
        return {};
    }

    const ShaderModule* Shader::GetShaderModule(const ShaderStageFlagBits stage) const
    {
        return m_ShaderModules.Single([&stage](const ShaderModule& module) { return module.GetStage() == stage; });
    }
}
