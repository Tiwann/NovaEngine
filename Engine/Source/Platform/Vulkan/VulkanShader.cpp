#include "VulkanShader.h"
#include "VulkanRenderer.h"
#include "Runtime/Application.h"
#include "Rendering/ShaderCompiler.h"
#include <slang/slang.h>


namespace Nova
{
    VulkanShader::VulkanShader(Renderer* Renderer, const String& Name, const Path& Filepath) : Shader(Renderer, Name, Filepath)
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

    VulkanShader::~VulkanShader()
    {
        const VulkanRenderer* Renderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkFunctionPointers& Functions = Renderer->GetFunctionPointers();
    }

    bool VulkanShader::Compile()
    {
        if (!m_Compiler)
            return false;
        
        m_ShaderModule = m_Compiler->loadModuleFromSource(*m_Name, m_Filepath.string().c_str(), nullptr, nullptr);
        if (!m_ShaderModule)
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

    bool VulkanShader::Link()
    {
        if (!m_Compiler) return false;

        Array<slang::IComponentType*> EntryPoints;
        for (const ShaderModule& Module : m_ShaderModules)
            EntryPoints.Add(Module.EntryPoint);

        if (SLANG_FAILED(m_Compiler->createCompositeComponentType(EntryPoints.Data(), EntryPoints.Count(), &m_Program, nullptr)))
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


    bool VulkanShader::Validate()
    {
        if (!m_LinkedProgram) return false;

        const VulkanRenderer* Renderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkDevice Device = Renderer->GetDevice();

        for (size_t i = 0; i < m_ShaderModules.Count(); i++)
        {
            m_LinkedProgram->getEntryPointCode(i, 0, &m_ShaderModules[i].CompiledCode);
        }

        const VkFunctionPointers& FunctionPtrs = Renderer->GetFunctionPointers();

        for (ShaderModule& Module : m_ShaderModules)
        {
            VkShaderCreateInfoEXT ShaderCreateInfo = { VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT };
            ShaderCreateInfo.stage = VulkanRenderer::ConvertShaderStage(Module.Stage);
            ShaderCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
            ShaderCreateInfo.pName = "main";
            ShaderCreateInfo.pCode = Module.CompiledCode->getBufferPointer();
            ShaderCreateInfo.codeSize = Module.CompiledCode->getBufferSize();
            ShaderCreateInfo.pSetLayouts = nullptr;
            ShaderCreateInfo.setLayoutCount = 0;
            ShaderCreateInfo.pPushConstantRanges = nullptr;
            ShaderCreateInfo.pushConstantRangeCount = 0;

            if (VK_FAILED(FunctionPtrs.vkCreateShadersEXT(Device, 1, &ShaderCreateInfo, nullptr, &Module.Handle)))
            {
                NOVA_VULKAN_ERROR("Failed to create Vulkan shaders with EXT_shader_object.");
                return false;
            }
        }
        return true;
    }

    bool VulkanShader::Bind()
    {
        const VulkanRenderer* Renderer =dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkCommandBuffer Cmd = Renderer->GetCurrentCommandBuffer();
        const VkFunctionPointers& Functions = Renderer->GetFunctionPointers();

        Array<VkShaderStageFlagBits> ShaderStages;
        Array<VkShaderEXT> Handles;
        for (const ShaderModule& Module : m_ShaderModules)
        {
            ShaderStages.Add(VulkanRenderer::ConvertShaderStage(Module.Stage));
            Handles.Add(Module.Handle);
        }

        Functions.vkCmdBindShadersEXT(Cmd, ShaderStages.Count(), ShaderStages.Data(), Handles.Data());
        return true;
    }

    void VulkanShader::Delete()
    {
    }

    i32 VulkanShader::GetUniformLocation(const String& Name)
    {
        return 0;
    }

    String VulkanShader::GetAssetType() const
    {
        return Shader::GetAssetType();
    }

    void VulkanShader::SetDirectionalLight(const String& Name, const DirectionalLight* DirLight)
    {
        Shader::SetDirectionalLight(Name, DirLight);
    }

    void VulkanShader::SetPointLight(const String& Name, const PointLight* PointLight)
    {
        Shader::SetPointLight(Name, PointLight);
    }

    void VulkanShader::SetAmbientLight(const String& Name, const AmbientLight* AmbientLight)
    {
        Shader::SetAmbientLight(Name, AmbientLight);
    }

    void VulkanShader::SetUniformFloat(const String& Name, f32 Value)
    {
        Shader::SetUniformFloat(Name, Value);
    }

    void VulkanShader::SetUniformFloat2(const String& Name, const Vector2& Value)
    {
        Shader::SetUniformFloat2(Name, Value);
    }

    void VulkanShader::SetUniformFloat3(const String& Name, const Vector3& Value)
    {
        Shader::SetUniformFloat3(Name, Value);
    }

    void VulkanShader::SetUniformFloat4(const String& Name, const Vector4& Value)
    {
        Shader::SetUniformFloat4(Name, Value);
    }

    void VulkanShader::SetUniformMat4(const String& Name, const Matrix4& Value)
    {
        Shader::SetUniformMat4(Name, Value);
    }

    void VulkanShader::SetUniformInt(const String& Name, i32 Value)
    {
        Shader::SetUniformInt(Name, Value);
    }

    void VulkanShader::SetUniformTexture(const String& Name, Texture2D* Texture)
    {
        Shader::SetUniformTexture(Name, Texture);
    }

    void VulkanShader::SetUniformMat2(const String& Name, const Matrix2& Value)
    {
        Shader::SetUniformMat2(Name, Value);
    }

    void VulkanShader::SetUniformMat3(const String& Name, const Matrix3& Value)
    {
        Shader::SetUniformMat3(Name, Value);
    }

    f32 VulkanShader::GetUniformFloat(const String& Name)
    {
        return Shader::GetUniformFloat(Name);
    }

    Vector2 VulkanShader::GetUniformFloat2(const String& Name)
    {
        return Shader::GetUniformFloat2(Name);
    }

    Vector3 VulkanShader::GetUniformFloat3(const String& Name)
    {
        return Shader::GetUniformFloat3(Name);
    }

    Vector4 VulkanShader::GetUniformFloat4(const String& Name)
    {
        return Shader::GetUniformFloat4(Name);
    }

    Matrix4 VulkanShader::GetUniformMat4(const String& Name)
    {
        return Shader::GetUniformMat4(Name);
    }

    i32 VulkanShader::GetUniformInt(const String& Name)
    {
        return Shader::GetUniformInt(Name);
    }

    static SlangStage ConvertShaderStage(ShaderStage Stage)
    {
        switch (Stage) {
        case ShaderStage::None: return SLANG_STAGE_FRAGMENT;
        case ShaderStage::Vertex: return SLANG_STAGE_VERTEX;
        case ShaderStage::Geometry: return SLANG_STAGE_GEOMETRY;
        case ShaderStage::Fragment: return SLANG_STAGE_FRAGMENT;
        default: throw;
        }

    }

    bool VulkanShader::FindShaderStage(const StringView& Name, ShaderStage Stage)
    {
        slang::IEntryPoint* EntryPoint = nullptr;
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint(*Name, ConvertShaderStage(Stage), &EntryPoint, nullptr)))
            return false;

        if (EntryPoint)
        {
            ShaderModule Module;
            Module.Stage = Stage;
            Module.EntryPoint = EntryPoint;
            m_ShaderModules.Add(Module);

            NOVA_LOG(Application, Verbosity::Info, "Shader {}: Found entry point {}.", GetName(), Name);
        }
        return true;
    }
}
