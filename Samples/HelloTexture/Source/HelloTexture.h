#pragma once
#include "Runtime/Application.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(HelloTexture, "HELLO TEXTURE")

namespace Nova
{
    class Pipeline;
    class Shader;
    class VertexBuffer;
    class IndexBuffer;
    class Texture2D;

    class HelloTexture final : public Application
    {
    public:
        explicit HelloTexture(const Array<const char*>& Arguments);
        ApplicationConfiguration CreateConfiguration() const override;
        void OnInit() override;
        void OnExit() override;
        void OnUpdate(float DeltaTime) override;
        void OnFrameStarted(Renderer* Renderer) override;
        void OnRender(Renderer* Renderer) override;
    private:
        Pipeline* m_Pipeline = nullptr;
        Shader* m_Shader = nullptr;
        VertexBuffer* m_VertexBuffer = nullptr;
        IndexBuffer* m_IndexBuffer = nullptr;
        Texture2D* m_Texture = nullptr;
    };
}
