#include "HelloTriangleApp.h"
#include "Editor/InspectorWindow.h"
#include "Rendering/CommandBuffer.h"
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
    SetupPipeline();
    GetEditorWindow<InspectorWindow>()->Hide();
    GetEditorWindow<EditorWindow>()->Hide();
}

void HelloTriangleApp::OnDestroy()
{
    m_GraphicsPipeline->Destroy();
    m_Shader->Destroy();
}

void HelloTriangleApp::OnRender(Nova::CommandBuffer& cmdBuffer)
{
    cmdBuffer.ClearColor(Color::Black, 0);
    cmdBuffer.BindGraphicsPipeline(*m_GraphicsPipeline);
    cmdBuffer.SetViewport(0.0f, 0.0f, GetWindowWidth(), GetWindowHeight(), 0.0f, 1.0f);
    cmdBuffer.SetScissor(0, 0, GetWindowWidth(), GetWindowHeight());
    cmdBuffer.Draw(3, 1, 0, 0);
}

Nova::RenderDeviceType HelloTriangleApp::GetRenderDeviceType() const
{
    return RenderDeviceType::D3D12;
}

void HelloTriangleApp::SetupPipeline()
{
    Ref<RenderDevice> renderDevice = GetRenderDevice();

    ShaderCreateInfo shaderCreateInfo;
    shaderCreateInfo.moduleInfo.name = "HelloTriangle";
    shaderCreateInfo.entryPoints.Add(ShaderEntryPoint::DefaultVertex());
    shaderCreateInfo.entryPoints.Add(ShaderEntryPoint::DefaultFragment());
    shaderCreateInfo.moduleInfo.filepath = Path::GetAssetPath("Shaders/HelloTriangle.slang");
    m_Shader = renderDevice->CreateShader(shaderCreateInfo);
    if (!m_Shader)
    {
        NOVA_LOG(HelloTriangle, Verbosity::Error, "Failed to create shader.");
        Exit();
    }


    VertexLayout vertexLayout;
    vertexLayout.AddInputBinding(0, VertexInputRate::Vertex);
    vertexLayout.AddInputAttribute("POSITION", ShaderDataType::Float3, 0);
    vertexLayout.AddInputAttribute("COLOR", ShaderDataType::Float4, 0);

    GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.shader = m_Shader;
    pipelineCreateInfo.colorAttachmentFormats = { Format::R8G8B8A8_SRGB };
    pipelineCreateInfo.depthAttachmentFormat = Format::D32_FLOAT_S8_UINT;
    pipelineCreateInfo.depthStencilState.depthTestEnable = false;
    pipelineCreateInfo.depthStencilState.stencilTestEnable = false;
    pipelineCreateInfo.depthStencilState.depthWriteEnable = false;
    pipelineCreateInfo.vertexInputState = CreateInputStateFromVertexLayout(vertexLayout);
    pipelineCreateInfo.SetMultisampleInfo({8});
    pipelineCreateInfo.colorBlendStates.Add(ColorBlendState{false});
    m_GraphicsPipeline = renderDevice->CreateGraphicsPipeline(pipelineCreateInfo);
    if (!m_GraphicsPipeline)
    {
        NOVA_LOG(HelloTriangle, Verbosity::Error, "Failed to create graphics pipeline.");
        Exit();
    }
}
