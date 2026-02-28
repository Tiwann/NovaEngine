#include "Shader.h"
#include "Rendering/ShaderModule.h"
#include "Runtime/Application.h"
#include "Rendering/SlangCommon.h"
#include "Runtime/Log.h"
#include <glad/glad.h>

namespace Nova::OpenGL
{
    bool Shader::Initialize(const ShaderCreateInfo& createInfo)
    {
        if (!createInfo.device) return false;
        const Application& application = Application::GetCurrentApplication();
        slang::IGlobalSession* slangSession = application.GetSlangSession();

        slang::TargetDesc shaderTargetDesc;
        shaderTargetDesc.format = GetCompileTarget(createInfo.target);
        shaderTargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;
        shaderTargetDesc.lineDirectiveMode = SLANG_LINE_DIRECTIVE_MODE_DEFAULT;
        shaderTargetDesc.profile = slangSession->findProfile("spirv_1_6");

        slang::CompilerOptionEntry options[] = {
            {slang::CompilerOptionName::MinimumSlangOptimization, slang::CompilerOptionValue(slang::CompilerOptionValueKind::Int, 1)},
            {slang::CompilerOptionName::MatrixLayoutColumn, slang::CompilerOptionValue(slang::CompilerOptionValueKind::Int, true)},
            {slang::CompilerOptionName::Optimization, slang::CompilerOptionValue(slang::CompilerOptionValueKind::Int, SLANG_OPTIMIZATION_LEVEL_NONE)},
        };

        const Array<const char*> includes = createInfo.includes.Transform<const char*>(
            [](const StringView includeDir) { return *includeDir; });

        slang::SessionDesc sessionDesc;
        sessionDesc.targets = &shaderTargetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
        sessionDesc.searchPaths = includes.Data();
        sessionDesc.searchPathCount = includes.Count();
        sessionDesc.compilerOptionEntries = options;
        sessionDesc.compilerOptionEntryCount = std::size(options);

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
            if (!shaderModule.Initialize(shaderModuleCreateInfo))
                return false;

            m_ShaderModules.Add(shaderModule);
        }

        m_ProgramId = glCreateProgram();
        for (const ShaderModule& module : m_ShaderModules)
            glAttachShader(m_ProgramId, module.GetHandle());

        glLinkProgram(m_ProgramId);
        GLint linkSuccess = false;
        glGetProgramiv(m_ProgramId, GL_LINK_STATUS, &linkSuccess);
        if (!linkSuccess)
        {
            GLint size = 0;
            glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &size);

            String message(size);
            glGetProgramInfoLog(m_ProgramId, size, nullptr, message.Data());

            NOVA_LOG(RenderDevice, Verbosity::Error, "Shader linking failed: {}", message);
            return false;
        }

        glValidateProgram(m_ProgramId);
        GLint validationSuccess = false;
        glGetProgramiv(m_ProgramId, GL_VALIDATE_STATUS, &validationSuccess);
        if (!validationSuccess)
        {
            GLint size = 0;
            glGetProgramiv(m_ProgramId, GL_INFO_LOG_LENGTH, &size);

            String message(size);
            glGetProgramInfoLog(m_ProgramId, size, nullptr, message.Data());

            NOVA_LOG(RenderDevice, Verbosity::Error, "Shader validation failed: {}", message);
            return false;
        }

        return true;
    }

    void Shader::Destroy()
    {
    }

    Ref<ShaderBindingSet> Shader::CreateBindingSet(size_t setIndex) const
    {
        return nullptr;
    }

    Array<Ref<ShaderBindingSet>> Shader::CreateBindingSets() const
    {
        return {};
    }
}
