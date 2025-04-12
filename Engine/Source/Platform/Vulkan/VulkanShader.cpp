#include "VulkanShader.h"

#include "VulkanRenderer.h"
#include "Runtime/Application.h"
#include "Rendering/ShaderCompiler.h"


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
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint("VertexMain", SLANG_STAGE_VERTEX, &m_VertexEntryPoint, nullptr)))
        {
            NOVA_LOG(Application, Verbosity::Error, "Failed to check vertex entry point!");
            return false;
        }
        
        if (SLANG_FAILED(m_ShaderModule->findAndCheckEntryPoint("FragmentMain", SLANG_STAGE_FRAGMENT, &m_FragmentEntryPoint, nullptr)))
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


    struct ReflectedShaderInfo
    {
        Array<VkDescriptorSetLayout> DescriptorSetLayouts;
        Array<VkPushConstantRange> PushConstantRanges;
    };

    bool ReflectShaderResources(slang::IComponentType* program, ReflectedShaderInfo& outInfo, VkDevice Device)
    {
        slang::ProgramLayout* layout = program->getLayout();
        if (!layout)
        {
            NOVA_LOG(Shader, Verbosity::Error, "Failed to get shader layout from Slang.");
            return false;
        }

        const u32 paramCount = layout->getParameterCount();
        for (u32 i = 0; i < paramCount; ++i)
        {
            slang::VariableLayoutReflection* var = layout->getParameterByIndex(i);
            if (!var) continue;

            slang::TypeReflection* type = var->getType();
            if (!type) continue;

            const SlangStage stage = var->getStage();
            VkShaderStageFlags vkStage = 0;
            if (stage & SLANG_STAGE_VERTEX) vkStage |= VK_SHADER_STAGE_VERTEX_BIT;
            if (stage & SLANG_STAGE_FRAGMENT) vkStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
            if (stage & SLANG_STAGE_COMPUTE) vkStage |= VK_SHADER_STAGE_COMPUTE_BIT;

            VkDescriptorSetLayoutBinding binding = {};
            binding.binding = var->getBindingIndex();
            binding.stageFlags = vkStage;
            binding.descriptorCount = 1;

            switch (type->getKind())
            {
            case slang::TypeReflection::Kind::ConstantBuffer:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case slang::TypeReflection::Kind::SamplerState:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                break;
            case slang::TypeReflection::Kind::Resource:
            case slang::TypeReflection::Kind::TextureBuffer:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                break;
            case slang::TypeReflection::Kind::ShaderStorageBuffer:
                binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            default:
                continue; // Skip unsupported types for now
            }

            VkDescriptorSetLayoutCreateInfo layoutInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
            layoutInfo.bindingCount = 1;
            layoutInfo.pBindings = &binding;

            VkDescriptorSetLayout layoutVk = nullptr;
            if (VK_FAILED(vkCreateDescriptorSetLayout(Device, &layoutInfo, nullptr, &layoutVk)))
            {
                const char* varName = var->getName();
                NOVA_LOG(Shader, Verbosity::Error, "Failed to create descriptor set layout for [{}]!",
                         varName ? varName : "Unknown");
                return false;
            }

            outInfo.DescriptorSetLayouts.Add(layoutVk);
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

        ReflectedShaderInfo shaderInfo;
        if (!ReflectShaderResources(m_LinkedProgram, shaderInfo, Device))
            return false;

        const VulkanRenderer* renderer = g_Application->GetRenderer<VulkanRenderer>();
        const VkDevice device = renderer->GetDevice();
        const VkFunctionPointers& vkFns = renderer->GetFunctionPointers();

        // Create Vulkan ShaderEXTs
        VkShaderCreateInfoEXT infos[2] = {};
        infos[0].sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
        infos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        infos[0].codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
        infos[0].pName = "main";
        infos[0].pCode = CompiledVertexCode->getBufferPointer();
        infos[0].codeSize = CompiledVertexCode->getBufferSize();
        infos[0].pSetLayouts = shaderInfo.DescriptorSetLayouts.Data();
        infos[0].setLayoutCount = shaderInfo.DescriptorSetLayouts.Count();
        infos[0].pPushConstantRanges = shaderInfo.PushConstantRanges.Data();
        infos[0].pushConstantRangeCount = shaderInfo.PushConstantRanges.Count();

        infos[1] = infos[0];
        infos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        infos[1].pCode = CompiledFragmentCode->getBufferPointer();
        infos[1].codeSize = CompiledFragmentCode->getBufferSize();

        VkShaderEXT handles[2] = {};
        if (VK_FAILED(vkFns.vkCreateShadersEXT(device, 2, infos, nullptr, handles)))
        {
            NOVA_VULKAN_ERROR("Failed to create Vulkan shaders with EXT_shader_object.");
            return false;
        }

        m_VertexHandle = handles[0];
        m_FragmentHandle = handles[1];
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
