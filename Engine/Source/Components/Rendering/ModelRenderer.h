#pragma once
#include "Runtime/Component.h"

namespace Nova
{
    class Model;
    class VertexBuffer;
    class VertexArray;
    class IndexBuffer;
    class Shader;
}

namespace Nova
{
    class ModelRenderer : public Component
    {
    public:
        ModelRenderer(Entity* Owner);
        void OnInit() override;
        void OnDestroy() override;
        void OnRender(Renderer* Renderer) override;
        void OnInspectorGUI(const ImGuiIO& IO) override;

        void OnModelChange(Model* NewModel);
        bool OpenFile();
        Model* GetModel() const;
    private:
        Model* m_Model{nullptr};
        Shader* m_Shader{nullptr};
    };
}
