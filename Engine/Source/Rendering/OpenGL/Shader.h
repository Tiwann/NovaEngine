#pragma once
#include "ShaderModule.h"
#include "Rendering/Shader.h"
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>


namespace Nova::OpenGL
{
    class Shader final : public Nova::Shader
    {
    public:
        bool Initialize(const ShaderCreateInfo& createInfo) override;
        void Destroy() override;
        Ref<ShaderBindingSet> CreateBindingSet(size_t setIndex) const override;
        Array<Ref<ShaderBindingSet>> CreateBindingSets() const override;

        uint32_t GetHandle() const { return m_Handle; }
    private:
        Slang::ComPtr<slang::ISession> m_Session = nullptr;
        Slang::ComPtr<slang::IModule> m_Module = nullptr;
        Array<Slang::ComPtr<slang::IEntryPoint>> m_EntryPoints;
        Slang::ComPtr<slang::IComponentType> m_Program = nullptr;
        Slang::ComPtr<slang::IComponentType> m_LinkedProgram = nullptr;
        Array<ShaderModule> m_ShaderModules;
        uint32_t m_Handle = INVALID_HANDLE<uint32_t>;
    };
}
