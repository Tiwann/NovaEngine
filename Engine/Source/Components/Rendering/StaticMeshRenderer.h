#pragma once
#include "Rendering/Sampler.h"
#include "Runtime/Component.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class StaticMesh;
    class Device;
    class GraphicsPipeline;
    class Buffer;
    class Shader;
    class ShaderBindingSet;
}

namespace Nova
{
    class StaticMeshRenderer final : public Component
    {
    public:
        explicit StaticMeshRenderer(Entity* owner);
        void OnInit() override;
        void OnDestroy() override;

        void OnPreRender(CommandBuffer& cmdBuffer) override;
        void OnRender(CommandBuffer& cmdBuffer) override;

        Ref<StaticMesh> GetStaticMesh() const;
        void SetStaticMesh(const Ref<StaticMesh>& newMesh);
    private:
        Ref<StaticMesh> m_StaticMesh = nullptr;
        Ref<Shader> m_Shader = nullptr;
        Ref<Sampler> m_Sampler = nullptr;
        Ref<GraphicsPipeline> m_Pipeline = nullptr;
        Ref<Buffer> m_SceneUniformBuffer = nullptr;
        Ref<ShaderBindingSet> m_BindingSet = nullptr;
    };
}
