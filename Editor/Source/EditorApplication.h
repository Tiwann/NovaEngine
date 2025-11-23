#pragma once
#include "Runtime/Application.h"

namespace Nova
{
    class EditorApplication final : public Application
    {
    public:
        ApplicationConfiguration GetConfiguration() const override;
        void OnInit() override;
        void OnUpdate(float deltaTime) override;
        void OnGUI() override;
        void OnRender(CommandBuffer& cmdBuffer) override;
    };
}
