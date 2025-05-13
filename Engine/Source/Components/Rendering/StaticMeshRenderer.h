#pragma once
#include "Runtime/Component.h"

namespace Nova
{
    class Pipeline;
}

namespace Nova
{
    class StaticMesh;
    class VertexBuffer;
    class VertexArray;
    class IndexBuffer;
    class Shader;
    class UniformBuffer;
}

namespace Nova
{
    class StaticMeshRenderer : public Component
    {
    public:
        StaticMeshRenderer(Entity* Owner);
        void OnInit() override;
        void OnDestroy() override;

        void OnFrameBegin(Renderer* Renderer) override;
        void OnRender(Renderer* Renderer) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;

        void OnModelChange(StaticMesh* NewModel);
        bool OpenFile();
        StaticMesh* GetStaticMesh() const;
        void SetStaticMesh(StaticMesh* NewStaticMesh);
    private:
        StaticMesh* m_StaticMesh = nullptr;
        Pipeline* m_Pipeline = nullptr;
        UniformBuffer* m_SceneUniformBuffer = nullptr;
    };
}
