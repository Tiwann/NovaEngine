#pragma once
#include "Rendering/Shader.h"
#include "ShaderBindingSetLayout.h"
#include "ShaderBindingSet.h"
#include "ShaderModule.h"
#include "Rendering/ShaderPushConstantRange.h"

typedef struct VkPipelineLayout_T* VkPipelineLayout;

namespace Nova::Vulkan
{
    class Device;

    class Shader final : public Nova::Shader
    {
    public:
        bool Initialize(const ShaderCreateInfo& createInfo) override;
        void Destroy() override;

        Ref<Nova::ShaderBindingSet> CreateBindingSet(size_t setIndex) const override;
        Array<Ref<Nova::ShaderBindingSet>> CreateBindingSets() const override;

        const Array<ShaderModule>& GetShaderModules() const;
        const ShaderModule& GetShaderModule(ShaderStageFlagBits stage) const;
        const Array<Ref<ShaderBindingSetLayout>>& GetBindingSetLayouts() const;
        ShaderStageFlags GetShaderStageFlags() const;
        VkPipelineLayout GetPipelineLayout() const;

        Array<VkDescriptorSetLayout> GetDescriptorSetLayouts() const;
    private:
        Device* m_Device = nullptr;
        Slang::ComPtr<slang::IGlobalSession> m_Slang = nullptr;
        Slang::ComPtr<slang::ISession> m_Session = nullptr;
        Slang::ComPtr<slang::IModule> m_Module = nullptr;
        Array<Slang::ComPtr<slang::IEntryPoint>> m_EntryPoints;
        Slang::ComPtr<slang::IComponentType> m_Program = nullptr;
        Slang::ComPtr<slang::IComponentType> m_LinkedProgram = nullptr;

        Array<ShaderPushConstantRange> m_PushConstantRanges;
        Array<ShaderModule> m_ShaderModules;
        Array<Ref<ShaderBindingSetLayout>> m_BindingSetLayouts;
        VkPipelineLayout m_PipelineLayout = nullptr;
    };
}
