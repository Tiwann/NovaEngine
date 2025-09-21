#pragma once
#include "Runtime/Component.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class StaticMesh;

    namespace Rendering
    {
        class Device;
        class GraphicsPipeline;
        class Buffer;
        class Shader;
        class ShaderBindingSet;
    }
}

namespace Nova
{
    class StaticMeshRenderer final : public Component
    {
    public:
        explicit StaticMeshRenderer(Entity* owner);
        void OnInit() override;
        void OnDestroy() override;

        void OnPreRender(Rendering::CommandBuffer& cmdBuffer) override;
        void OnRender(Rendering::CommandBuffer& cmdBuffer) override;

        Ref<StaticMesh> GetStaticMesh() const;
        void SetStaticMesh(Ref<StaticMesh> newMesh);
    private:
        Ref<StaticMesh> m_StaticMesh = nullptr;
        Ref<Rendering::Shader> m_Shader = nullptr;
        Ref<Rendering::GraphicsPipeline> m_Pipeline = nullptr;
        Ref<Rendering::Buffer> m_SceneUniformBuffer = nullptr;
        Ref<Rendering::ShaderBindingSet> m_BindingSet = nullptr;
    };
}
