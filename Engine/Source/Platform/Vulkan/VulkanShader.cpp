#include "VulkanShader.h"

#include "VulkanRenderer.h"
#include "Core/Application.h"
#include "Core/Graphics/ShaderCompiler.h"


namespace Nova
{
    VulkanShader::VulkanShader(const String& Name, const Path& Filepath) : Shader(Name, Filepath)
    {
        slang::IGlobalSession* Slang = g_Application->GetSlangSession();
        
        slang::TargetDesc TargetDesc = {};
        TargetDesc.format = SLANG_SPIRV;

        slang::SessionDesc SessionDesc = {};
        SessionDesc.targetCount = 1;
        SessionDesc.targets = &TargetDesc;
        if (SLANG_FAILED(Slang->createSession(SessionDesc, &m_Compiler)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to create Slang session!");
        }
    }

    bool VulkanShader::Compile()
    {
        if (!m_Compiler)
        {
            return false;
        }
        
        m_ShaderModule = m_Compiler->loadModuleFromSource(*m_Name, m_Filepath.string().c_str(), nullptr, nullptr);
        if (!m_ShaderModule)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to compile shader: Failed to load module");
        }
        
        // At the moment we assume that vertex shaders should have the attribute [shader("vertex")] and entry point should be named vsmain
        // At the moment we assume that fragment shaders should have the attribute [shader("fragment")] and entry point should be named fsmain
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint("vsmain", SLANG_STAGE_VERTEX, &m_VertexEntryPoint, nullptr)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to check vertex entry point!");
            return false;
        }
        
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint("fsmain", SLANG_STAGE_FRAGMENT, &m_FragmentEntryPoint, nullptr)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to check fragment entry point!");
            return false;
        }
        return true;
    }

    bool VulkanShader::Link()
    {
        if (!m_Compiler) return false;
        
        slang::IComponentType* const ShaderStages[] { m_ShaderModule, m_VertexEntryPoint, m_FragmentEntryPoint };
        if (SLANG_FAILED(m_Compiler->createCompositeComponentType(ShaderStages, std::size(ShaderStages), &m_Program, nullptr)))
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

        const VulkanRenderer* Renderer = g_Application->GetRenderer<VulkanRenderer>();
        const VkDevice Device = Renderer->GetDevice();
        
        slang::IBlob* CompiledVertexCode = nullptr;
        slang::IBlob* CompiledFragmentCode = nullptr;
        m_LinkedProgram->getEntryPointCode(0, 0, &CompiledVertexCode);
        m_LinkedProgram->getEntryPointCode(1, 0, &CompiledFragmentCode);
        
        slang::ProgramLayout* Layout = m_Program->getLayout();
        if (!Layout)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to get layout!");
            return false;
        }

        
        Array<VkDescriptorSetLayout> Layouts = { Layout->getParameterCount() };
        for (size_t i = 0; i < Layouts.Count(); ++i)
        {
            slang::VariableLayoutReflection* Variable = Layout->getParameterByIndex((SlangUInt32)i);
            switch (Variable->getType()->getKind())
            {
            case slang::TypeReflection::Kind::None:
                break;
            case slang::TypeReflection::Kind::Struct:
                break;
            case slang::TypeReflection::Kind::Array:
                break;
            case slang::TypeReflection::Kind::Matrix:
                break;
            case slang::TypeReflection::Kind::Vector:
                break;
            case slang::TypeReflection::Kind::Scalar:
                break;
            case slang::TypeReflection::Kind::ConstantBuffer:
                break;
            case slang::TypeReflection::Kind::Resource:
                break;
            case slang::TypeReflection::Kind::SamplerState:
                break;
            case slang::TypeReflection::Kind::TextureBuffer:
                break;
            case slang::TypeReflection::Kind::ShaderStorageBuffer:
                break;
            case slang::TypeReflection::Kind::ParameterBlock:
                break;
            case slang::TypeReflection::Kind::GenericTypeParameter:
                break;
            case slang::TypeReflection::Kind::Interface:
                break;
            case slang::TypeReflection::Kind::OutputStream:
                break;
            case slang::TypeReflection::Kind::Specialized:
                break;
            case slang::TypeReflection::Kind::Feedback:
                break;
            case slang::TypeReflection::Kind::Pointer:
                break;
            case slang::TypeReflection::Kind::DynamicResource:
                break;
            }
            
            VkDescriptorSetLayoutBinding Binding = { };
            Binding.binding = 0;
            Binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            Binding.descriptorCount = 1;
            Binding.stageFlags = Variable->getStage() == SLANG_STAGE_VERTEX ? VK_SHADER_STAGE_VERTEX_BIT
            : Variable->getStage() == SLANG_STAGE_FRAGMENT ? VK_SHADER_STAGE_FRAGMENT_BIT : 0;

            VkDescriptorSetLayoutCreateInfo SetLayoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            SetLayoutInfo.bindingCount = 1;
            SetLayoutInfo.pBindings = &Binding;

            if (VK_FAILED(vkCreateDescriptorSetLayout(Device, &SetLayoutInfo, nullptr, &Layouts[i])))
            {
                NOVA_LOG(Shader, Verbosity::Error, "Failed to create descriptor layout ({})!", i);
                return false;
            }
        }
        
        
        // Need to reflect on shaders to get Layouts and push constants
        VkShaderCreateInfoEXT VertexShaderCreateInfo = { VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT };
        VertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        VertexShaderCreateInfo.nextStage = VK_SHADER_STAGE_FRAGMENT_BIT;
        VertexShaderCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
        VertexShaderCreateInfo.codeSize = CompiledVertexCode->getBufferSize();
        VertexShaderCreateInfo.pCode = CompiledVertexCode->getBufferPointer();
        VertexShaderCreateInfo.pName = "main";
        VertexShaderCreateInfo.pSetLayouts = Layouts.Data();
        VertexShaderCreateInfo.setLayoutCount = (u32)Layouts.Count();
        VertexShaderCreateInfo.pPushConstantRanges = nullptr;
        VertexShaderCreateInfo.pushConstantRangeCount = 0;

        VkShaderCreateInfoEXT FragmentShaderCreateInfo = { VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT };
        FragmentShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        FragmentShaderCreateInfo.nextStage = 0;
        FragmentShaderCreateInfo.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
        FragmentShaderCreateInfo.codeSize = CompiledFragmentCode->getBufferSize();
        FragmentShaderCreateInfo.pCode = CompiledFragmentCode->getBufferPointer();
        FragmentShaderCreateInfo.pName = "main";
        FragmentShaderCreateInfo.pSetLayouts = Layouts.Data();
        FragmentShaderCreateInfo.setLayoutCount = (u32)Layouts.Count();
        FragmentShaderCreateInfo.pPushConstantRanges = nullptr;
        FragmentShaderCreateInfo.pushConstantRangeCount = 0;

        const VkShaderCreateInfoEXT CreateInfos[2] = { VertexShaderCreateInfo, FragmentShaderCreateInfo };
        
        const VkFunctionPointers& Functions = Renderer->GetFunctionPointers();
        if (VK_FAILED(Functions.vkCreateShadersEXT(Device, (u32)std::size(CreateInfos), CreateInfos, nullptr, &m_VertexHandle)))
        {
            NOVA_VULKAN_ERROR("Failed to create vulkan shaders!");
            return false;
        }
        
        return true;
    }

    bool VulkanShader::Bind()
    {
        const VulkanRenderer* Renderer = g_Application->GetRenderer<VulkanRenderer>();
        const VkCommandBuffer Cmd = Renderer->GetCurrentCommandBuffer();
        const VkFunctionPointers& Functions = Renderer->GetFunctionPointers();

        const VkShaderStageFlagBits ShaderStages[2] = { VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT };
        const VkShaderEXT Shaders[2] = { m_VertexHandle, m_FragmentHandle };
        Functions.vkCmdBindShadersEXT(Cmd, (u32)std::size(ShaderStages), ShaderStages, Shaders);
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

    bool VulkanShader::Reload()
    {
        return Shader::Reload();
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
}
