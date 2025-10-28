#include "Material.h"

#include "Device.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/Shader.h"
#include "Rendering/ShaderBindingSet.h"
#include "Runtime/Application.h"

namespace Nova::Vulkan
{
    bool Material::Build()
    {
        if (!m_Shader) return false;

        if (m_Pipeline)
            m_Pipeline->Destroy();

        for (auto& bindingSet : m_BindingSets)
            bindingSet->Destroy();
        m_BindingSets.Clear();

        m_BindingSets = m_Shader->CreateBindingSets();

        Rendering::GraphicsPipelineCreateInfo pipelineInfo = Rendering::GraphicsPipelineCreateInfo()
            .setShader(m_Shader)
            .setPrimitiveTopology(PrimitiveTopology::TriangleList);

        auto& device = Application::GetCurrentApplication().GetDevice();
        m_Pipeline = device->CreateGraphicsPipeline(pipelineInfo);
        if (!m_Pipeline) return false;

        return true;
    }

    void Material::BindTexture(StringView name, Ref<Rendering::Texture> texture)
    {
    }

    void Material::BindBuffer(StringView name, Ref<Rendering::Buffer> buffer)
    {
    }
}
