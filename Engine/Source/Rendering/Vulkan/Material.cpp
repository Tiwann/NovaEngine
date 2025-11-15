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

        if (!m_BindingSets.IsEmpty())
        {
            for (Ref<Rendering::ShaderBindingSet>& bindingSet : m_BindingSets)
                bindingSet->Destroy();
            m_BindingSets.Clear();
        }

        m_BindingSets = m_Shader->CreateBindingSets();
        if (m_BindingSets.IsEmpty()) return false;
        return true;
    }

    void Material::Destroy()
    {
        if (!m_BindingSets.IsEmpty())
        {
            for (Ref<Rendering::ShaderBindingSet>& bindingSet : m_BindingSets)
                bindingSet->Destroy();
            m_BindingSets.Clear();
        }

        m_Shader = nullptr;
    }

    void Material::SetSampler(const StringView name, Ref<Rendering::Sampler> sampler)
    {
        const Array<ShaderBindingSetLayout>& setLayouts = m_Shader.As<Shader>()->GetBindingSetLayouts();

        for (uint32_t setIndex = 0; setIndex < setLayouts.Count(); setIndex++)
        {
            const ShaderBindingSetLayout& setLayout = setLayouts[setIndex];
            const ShaderBindingSetLayout::BindingMap& bindings = setLayout.GetBindings();

            for (size_t i = 0; i < bindings.Count(); i++)
            {
                const auto& binding = bindings.GetAt(i);

                if (name == binding.value.name)
                {
                    m_BindingSets[setIndex]->BindSampler(binding.key, sampler);
                    return;
                }
            }
        }
    }

    void Material::SetTexture(const StringView name, Ref<Rendering::Texture> texture)
    {
        const Array<ShaderBindingSetLayout>& setLayouts = m_Shader.As<Shader>()->GetBindingSetLayouts();

        for (uint32_t setIndex = 0; setIndex < setLayouts.Count(); setIndex++)
        {
            const ShaderBindingSetLayout& setLayout = setLayouts[setIndex];
            const ShaderBindingSetLayout::BindingMap& bindings = setLayout.GetBindings();

            for (size_t i = 0; i < bindings.Count(); i++)
            {
                const auto& binding = bindings.GetAt(i);

                if (name == binding.value.name)
                {
                    m_BindingSets[setIndex]->BindTexture(binding.key, texture);
                    return;
                }
            }
        }
    }

    void Material::SetSamplerAndTexture(const StringView name, Ref<Rendering::Sampler> sampler, Ref<Rendering::Texture> texture)
    {
        const Array<ShaderBindingSetLayout>& setLayouts = m_Shader.As<Shader>()->GetBindingSetLayouts();

        for (uint32_t setIndex = 0; setIndex < setLayouts.Count(); setIndex++)
        {
            const ShaderBindingSetLayout& setLayout = setLayouts[setIndex];
            const ShaderBindingSetLayout::BindingMap& bindings = setLayout.GetBindings();

            for (size_t i = 0; i < bindings.Count(); i++)
            {
                const auto& binding = bindings.GetAt(i);

                if (name == binding.value.name)
                {
                    m_BindingSets[setIndex]->BindCombinedSamplerTexture(binding.key, sampler, texture);
                    return;
                }
            }
        }
    }

    void Material::SetBuffer(const StringView name, Ref<Rendering::Buffer> buffer, const size_t offset, const size_t size)
    {
        const Array<ShaderBindingSetLayout>& setLayouts = m_Shader.As<Shader>()->GetBindingSetLayouts();

        for (uint32_t setIndex = 0; setIndex < setLayouts.Count(); setIndex++)
        {
            const ShaderBindingSetLayout& setLayout = setLayouts[setIndex];
            const ShaderBindingSetLayout::BindingMap& bindings = setLayout.GetBindings();

            for (size_t i = 0; i < bindings.Count(); i++)
            {
                const auto& binding = bindings.GetAt(i);

                if (name == binding.value.name)
                {
                    m_BindingSets[setIndex]->BindBuffer(binding.key, buffer, offset, size);
                    return;
                }
            }
        }
    }
}
