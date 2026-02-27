#include "HelloTriangleApp.h"

#include "Editor/InspectorWindow.h"
#include "Rendering/CommandBuffer.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/Shader.h"
#include "Runtime/Log.h"
#include "Runtime/LogCategory.h"
#include "Runtime/Path.h"
#include "Runtime/Time.h"

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
    shaderCreateInfo.target = ShaderTarget::SPIRV;

    m_Shader = renderDevice->CreateShader(shaderCreateInfo);
    if (!m_Shader)
    {
        NOVA_LOG(HelloTriangle, Verbosity::Error, "Failed to create shader.");
        Exit();
    }

    GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.shader = m_Shader;
    pipelineCreateInfo.colorAttachmentFormats = { Format::R8G8B8A8_SRGB };
    pipelineCreateInfo.depthAttachmentFormat = Format::D32_FLOAT_S8_UINT;
    pipelineCreateInfo.SetMultisampleInfo({8});
    m_GraphicsPipeline = renderDevice->CreateGraphicsPipeline(pipelineCreateInfo);
    if (!m_GraphicsPipeline)
    {
        NOVA_LOG(HelloTriangle, Verbosity::Error, "Failed to create graphics pipeline.");
        Exit();
    }

    GetEditorWindow<InspectorWindow>()->Hide();
    GetEditorWindow<EditorWindow>()->Hide();
}

void HelloTriangleApp::OnDestroy()
{
    m_GraphicsPipeline->Destroy();
    m_GraphicsPipeline.Release();
    m_Shader->Destroy();
    m_Shader.Release();
}

void HelloTriangleApp::OnPreRender(Nova::CommandBuffer& cmdBuffer)
{
    const float time = Time::Get();
    cmdBuffer.PushConstants(*m_Shader, ShaderStageFlagBits::Vertex, 0, sizeof(float), &time);
}

void HelloTriangleApp::OnRender(Nova::CommandBuffer& cmdBuffer)
{
    cmdBuffer.BindGraphicsPipeline(*m_GraphicsPipeline);
    cmdBuffer.SetViewport(0.0f, 0.0f, GetWindowWidth(), GetWindowHeight(), 0.0f, 1.0f);
    cmdBuffer.SetScissor(0, 0, GetWindowWidth(), GetWindowHeight());
    cmdBuffer.Draw(3, 1, 0, 0);
}

Nova::RenderDeviceType HelloTriangleApp::GetRenderDeviceType() const
{
    return RenderDeviceType::Vulkan;
}
