#pragma once
#include "Runtime/Application.h"

class HelloTriangleApp final : public Nova::Application
{
public:
    HelloTriangleApp(const int32_t argc, char** argv): Application(argc, argv){}

    Nova::ApplicationConfiguration GetConfiguration() const override;
    void OnInit() override;
    void OnDestroy() override;
    void OnRender(Nova::CommandBuffer& cmdBuffer) override;
    Nova::RenderDeviceType GetRenderDeviceType() const override;
private:
    Nova::Ref<Nova::Shader> m_Shader = nullptr;
    Nova::Ref<Nova::GraphicsPipeline> m_GraphicsPipeline = nullptr;
};