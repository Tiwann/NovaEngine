#pragma once
#include "Runtime/Application.h"

namespace Nova
{
    class EditorApplication final : public Application
    {
    public:
        explicit EditorApplication(const Array<const char*>& Arguments);
        ApplicationConfiguration CreateConfiguration(ApplicationConfigurationSerializer& Serializer) const override;
        void OnInit() override;
        void OnExit() override;
        void OnRender(Renderer* Renderer) override;
        void OnUpdate(f32 Delta) override;
    };
}
