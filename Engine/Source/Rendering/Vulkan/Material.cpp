#include "Material.h"
#include "Device.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/Shader.h"
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

    void Material::SetSampler(StringView name, Ref<Rendering::Sampler> sampler)
    {
    }

    void Material::SetTexture(StringView name, Ref<Rendering::Texture> texture)
    {
    }

    void Material::SetSamplerAndTexture(StringView name, Ref<Rendering::Sampler> sampler,
        Ref<Rendering::Texture> texture)
    {
    }

    void Material::SetBuffer(StringView name, Ref<Rendering::Buffer> buffer)
    {
    }
}
