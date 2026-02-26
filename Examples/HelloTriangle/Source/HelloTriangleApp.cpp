#include "HelloTriangleApp.h"

#include "Rendering/GraphicsPipeline.h"
#include "Rendering/Shader.h"
#include "Runtime/Log.h"
#include "Runtime/LogCategory.h"
#include "Runtime/Path.h"

using namespace Nova;

NOVA_DECLARE_LOG_CATEGORY_STATIC(HelloTriangle, "HelloTriangle")

extern "C" Application* CreateApplication(int argc, char** argv)
{
    return new HelloTriangleApp(argc, argv);
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
    Ref<RenderDevice> renderDevice = GetRenderDevice();

    ShaderCreateInfo shaderCreateInfo;
    shaderCreateInfo.entryPoints.Add(ShaderEntryPoint::DefaultVertex());
    shaderCreateInfo.entryPoints.Add(ShaderEntryPoint::DefaultFragment());
    shaderCreateInfo.moduleInfo.name = "HelloTriangle";
    shaderCreateInfo.moduleInfo.filepath = Path::GetAssetPath("Shaders/HelloTriangle.slang");
    shaderCreateInfo.slang = GetSlangSession();
    shaderCreateInfo.target = ShaderTarget::SPIRV;

    m_Shader = renderDevice->CreateShader(shaderCreateInfo);
    if (!m_Shader)
    {
        NOVA_LOG(HelloTriangle, Verbosity::Error, "Failed to create shader.");
        Exit();
    }

    GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.shader = m_Shader;
    pipelineCreateInfo.renderPass = GetRenderPass();
}

void HelloTriangleApp::OnDestroy()
{
    Application::OnDestroy();
}

void HelloTriangleApp::OnUpdate(const float deltaTime)
{

}

void HelloTriangleApp::OnRender(Nova::CommandBuffer& cmdBuffer)
{

}

Nova::RenderDeviceType HelloTriangleApp::GetRenderDeviceType() const
{
    return RenderDeviceType::Vulkan;
}
