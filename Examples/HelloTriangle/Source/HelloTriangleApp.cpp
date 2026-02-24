#include "HelloTriangleApp.h"
using namespace Nova;

extern "C" ::Nova::Application* CreateApplication(int argc, char** argv)
{
    return new ::HelloTriangleApp(argc, argv);
}

Nova::ApplicationConfiguration HelloTriangleApp::GetConfiguration() const
{
    ApplicationConfiguration configuration;
    configuration.applicationName = "HelloTriangle";
    configuration.vsync = true;
    configuration.windowFlags = WindowCreateFlagBits::Default;
    configuration.windowWidth = 800;
    configuration.windowHeight = 600;
    return configuration;
}

void HelloTriangleApp::OnInit()
{

}

void HelloTriangleApp::OnUpdate(const float deltaTime)
{

}

void HelloTriangleApp::OnRender(Nova::CommandBuffer& cmdBuffer)
{

}

Nova::DeviceType HelloTriangleApp::GetRenderDeviceType() const
{
#ifdef NOVA_HAS_D3D12
    return DeviceType::D3D12;
#else
    return DeviceType::Vulkan;
#endif
}
