#pragma once
#include "Rendering/Shader.h"
#include "Rendering/ShaderPushConstantRange.h"
#include "ShaderModule.h"
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>


namespace Nova::D3D12
{
    class Shader : public Nova::Shader
    {
    public:
        bool Initialize(const ShaderCreateInfo& createInfo) override;
        void Destroy() override;
        Ref<ShaderBindingSet> CreateBindingSet(size_t setIndex) const override;
        Array<Ref<ShaderBindingSet>> CreateBindingSets() const override;

        const ShaderModule* GetShaderModule(ShaderStageFlagBits stage) const;
    private:
        RenderDevice* m_Device = nullptr;
        Slang::ComPtr<slang::ISession> m_Session = nullptr;
        Slang::ComPtr<slang::IModule> m_Module = nullptr;
        Array<Slang::ComPtr<slang::IEntryPoint>> m_EntryPoints;
        Slang::ComPtr<slang::IComponentType> m_Program = nullptr;
        Slang::ComPtr<slang::IComponentType> m_LinkedProgram = nullptr;

        Array<ShaderPushConstantRange> m_PushConstantRanges;
        Array<ShaderModule> m_ShaderModules;
    };
}
