#pragma once
#include "Runtime/Asset.h"
#include "Runtime/Ref.h"
#include "Containers/Map.h"
#include "Containers/StringView.h"

namespace Nova::Rendering
{
    class Shader;
    class ShaderBindingSet;
    class Texture;
    class Buffer;
    class GraphicsPipeline;

    class Material : public Asset
    {
    public:
        Material(Ref<Shader> shader) : m_Shader(shader) {}

        String GetAssetType() const override { return "Material"; }

        virtual bool Build() = 0;
        virtual void BindTexture(StringView name, Ref<Texture> texture) = 0;
        virtual void BindBuffer(StringView name, Ref<Buffer> buffer) = 0;

        const Array<Ref<ShaderBindingSet>>& GetBindingSets() const { return m_BindingSets; }

        Ref<Shader> GetShader() const { return m_Shader; }
        Ref<GraphicsPipeline> GetPipeline() const { return m_Pipeline; }
    protected:
        Ref<Shader> m_Shader = nullptr;
        Array<Ref<ShaderBindingSet>> m_BindingSets;
        Ref<GraphicsPipeline> m_Pipeline = nullptr;
    };
}
