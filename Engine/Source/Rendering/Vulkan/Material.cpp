#include "Material.h"
#include "Device.h"
#include "Shader.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/ShaderBindingSet.h"
#include "Runtime/Application.h"

namespace Nova::Vulkan
{
    bool Material::Initialize(const Rendering::MaterialCreateInfo& createInfo)
    {
        if (!createInfo.device || !createInfo.shader) return false;

        m_Device = createInfo.device;
        m_Shader = createInfo.shader;

        if (m_BindingSet) m_BindingSet->Destroy();

        m_BindingSet = m_Shader->CreateBindingSet(1);
        if (!m_BindingSet) return false;
        return true;
    }

    void Material::Destroy()
    {
        m_BindingSet->Destroy();
        m_Shader = nullptr;
    }

    void Material::SetSampler(const StringView name, Ref<Rendering::Sampler> sampler)
    {
        const Array<ShaderBindingSetLayout>& setLayouts = m_Shader.As<Shader>()->GetBindingSetLayouts();
        const ShaderBindingSetLayout& setLayout = setLayouts[1];
        const ShaderBindingSetLayout::BindingMap& bindings = setLayout.GetBindings();

        for (size_t i = 0; i < bindings.Count(); i++)
        {
            const auto& binding = bindings.GetAt(i);

            if (name == binding.value.name)
            {
                m_BindingSet->BindSampler(binding.key, sampler);
                return;
            }
        }
    }

    void Material::SetTexture(const StringView name, Ref<Rendering::Texture> texture)
    {
        const Array<ShaderBindingSetLayout>& setLayouts = m_Shader.As<Shader>()->GetBindingSetLayouts();
        const ShaderBindingSetLayout& setLayout = setLayouts[1];
        const ShaderBindingSetLayout::BindingMap& bindings = setLayout.GetBindings();

        for (size_t i = 0; i < bindings.Count(); i++)
        {
            const auto& binding = bindings.GetAt(i);

            if (name == binding.value.name)
            {
                m_BindingSet->BindTexture(binding.key, texture);
                return;
            }
        }
    }

    void Material::SetSamplerAndTexture(const StringView name, Ref<Rendering::Sampler> sampler, Ref<Rendering::Texture> texture)
    {
        const Array<ShaderBindingSetLayout>& setLayouts = m_Shader.As<Shader>()->GetBindingSetLayouts();
        const ShaderBindingSetLayout& setLayout = setLayouts[1];
        const ShaderBindingSetLayout::BindingMap& bindings = setLayout.GetBindings();

        for (size_t i = 0; i < bindings.Count(); i++)
        {
            const auto& binding = bindings.GetAt(i);

            if (name == binding.value.name)
            {
                m_BindingSet->BindCombinedSamplerTexture(binding.key, sampler, texture);
                return;
            }
        }
    }

    void Material::SetBuffer(const StringView name, Ref<Rendering::Buffer> buffer, const size_t offset, const size_t size)
    {
        const Array<ShaderBindingSetLayout>& setLayouts = m_Shader.As<Shader>()->GetBindingSetLayouts();
        const ShaderBindingSetLayout& setLayout = setLayouts[1];
        const ShaderBindingSetLayout::BindingMap& bindings = setLayout.GetBindings();

        for (size_t i = 0; i < bindings.Count(); i++)
        {
            const auto& binding = bindings.GetAt(i);

            if (name == binding.value.name)
            {
                m_BindingSet->BindBuffer(binding.key, buffer, offset, size);
                return;
            }
        }
    }
}
