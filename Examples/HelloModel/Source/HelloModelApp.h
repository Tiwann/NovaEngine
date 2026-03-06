#pragma once
#include "Runtime/Application.h"
#include "Runtime/EntityHandle.h"

class HelloModelApp final : public Nova::Application
{
public:
    HelloModelApp(const int32_t argc, char** argv): Application(argc, argv){}

    Nova::ApplicationConfiguration GetConfiguration() const override;
    void OnInit() override;
    Nova::RenderDeviceType GetRenderDeviceType() const override;
private:
    Nova::EntityHandle m_ModelEntity = nullptr;
};
