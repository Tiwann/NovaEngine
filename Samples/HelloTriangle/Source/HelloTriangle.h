﻿#pragma once
#include "../../../Engine/Source/Runtime/Application.h"
#include "../../../Engine/Source/Runtime/LogCategory.h"
NOVA_DECLARE_LOG_CATEGORY_STATIC(NovaEditor, "HELLO TRIANGLE")

namespace Nova
{
    class HelloTriangle final : public Application
    {
    public:
        explicit HelloTriangle(const Array<const char*>& Arguments);
        ApplicationConfiguration CreateConfiguration() const override;
        void OnInit() override;
        void OnExit() override;
        void OnUpdate(float DeltaTime) override;
        void OnRender(Renderer* Renderer) override;
    private:
        Pipeline* m_Pipeline = nullptr;
        Shader* m_Shader = nullptr;
        VertexBuffer* m_VertexBuffer = nullptr;
        IndexBuffer* m_IndexBuffer = nullptr;
    };
}
