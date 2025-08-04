#include "Runtime/EntryPoint.h"
#include "Runtime/DesktopWindow.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/RenderTarget.h"
#include "Containers/StringView.h"
#include "Rendering/Vulkan/Buffer.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/Vulkan/GraphicsPipeline.h"
#include "Rendering/ShaderModule.h"
#include "Rendering/Vulkan/ShaderModule.h"
#include "Utils/ShaderUtils.h"
#include "Utils/BufferUtils.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Containers/StringFormat.h"
#include "Rendering/Vulkan/Conversions.h"
#include "Runtime/Time.h"
#include "Runtime/ScopedTimer.h"
#include "Rendering/RenderPass.h"
#include "Components/Camera.h"
#include "Game/Scene.h"
#include "Components/Transform.h"
#include "Rendering/Vulkan/ImGuiRenderer.h"

#include <vulkan/vulkan.h>
#include <imgui.h>
#include <cstdlib>

static constexpr uint32_t SAMPLE_COUNT = 8;
static bool g_Running = true;

namespace Nova
{
    static String GetAssetPath(const StringView filepath)
    {
        return StringFormat("{}/{}", APPLICATION_DIR, filepath);
    }

    int GuardedMain(int, char**)
    {
        WindowCreateInfo windowCreateInfo;
        windowCreateInfo.title = "Hello Triangle";
        windowCreateInfo.width = 1280;
        windowCreateInfo.height = 720;
        windowCreateInfo.show = true;

        DesktopWindow window;
        window.Initialize(windowCreateInfo);

        window.closeEvent.Bind([] { g_Running = false; });

        Rendering::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.applicationName = "Hello Triangle";
        deviceCreateInfo.versionMajor = 1;
        deviceCreateInfo.versionMinor = 0;
        deviceCreateInfo.window = &window;
        deviceCreateInfo.buffering = SwapchainBuffering::DoubleBuffering;
        deviceCreateInfo.vSync = false;

        Vulkan::Device device;
        if (!device.Initialize(deviceCreateInfo))
            return EXIT_FAILURE;
        device.SetName("Main Device");

        Rendering::RenderTargetCreateInfo renderTargetCreateInfo;
        renderTargetCreateInfo.device = &device;
        renderTargetCreateInfo.width = window.GetWidth();
        renderTargetCreateInfo.height = window.GetHeight();
        renderTargetCreateInfo.depth = 1;
        renderTargetCreateInfo.colorFormat = Format::R8G8B8A8_UNORM;
        renderTargetCreateInfo.depthFormat = Format::D32_FLOAT_S8_UINT;
        renderTargetCreateInfo.sampleCount = SAMPLE_COUNT;

        Vulkan::RenderTarget renderTarget;
        if (!renderTarget.Initialize(renderTargetCreateInfo))
            return EXIT_FAILURE;

        window.maximizeEvent.Bind([&device]
        {
            Vulkan::Swapchain* swapchain = device.GetSwapchain();
            swapchain->Invalidate();
        });

        window.resizeEvent.Bind([&device, &renderTarget](const int32_t newWidth, const int32_t newHeight)
        {
            Vulkan::Swapchain* swapchain = device.GetSwapchain();
            swapchain->Invalidate();
        });

        Rendering::ImGuiRendererCreateInfo imguiCreateInfo;
        imguiCreateInfo.device = &device;
        imguiCreateInfo.window = &window;
        imguiCreateInfo.sampleCount = SAMPLE_COUNT;
        Vulkan::ImGuiRenderer imgui;
        imgui.Initialize(imguiCreateInfo);

        Array<uint32_t> vertSpirv, fragSpirv;
        CompileShaderToSpirv(GetAssetPath("Shaders/HelloTriangle.slang"), "HelloTriangle", vertSpirv, fragSpirv);

        Vulkan::ShaderModule vertShaderModule = Rendering::ShaderModule::Create<Vulkan::ShaderModule>(device, ShaderStageFlagBits::Vertex, vertSpirv);
        Vulkan::ShaderModule fragShaderModule = Rendering::ShaderModule::Create<Vulkan::ShaderModule>(device, ShaderStageFlagBits::Fragment, fragSpirv);

        VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageCreateInfo.module = vertShaderModule.GetHandle();
        vertexShaderStageCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageCreateInfo.module = fragShaderModule.GetHandle();
        fragmentShaderStageCreateInfo.pName = "main";


        VkPushConstantRange range;
        range.offset = 0;
        range.size = sizeof(Matrix4);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = &range;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;

        VkPipelineLayout pipelineLayout;
        VkResult result = vkCreatePipelineLayout(device.GetHandle(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
        if (result != VK_SUCCESS)
            return EXIT_FAILURE;


        Rendering::RenderPassAttachment colorAttachment;
        colorAttachment.type = Rendering::AttachmentType::Color;
        colorAttachment.loadOp = Rendering::LoadOperation::Clear;
        colorAttachment.storeOp = Rendering::StoreOperation::Store;
        colorAttachment.clearValue.color = Color::Black;

        Rendering::RenderPassAttachment depthAttachment;
        depthAttachment.type = Rendering::AttachmentType::Depth;
        depthAttachment.loadOp = Rendering::LoadOperation::Clear;
        depthAttachment.storeOp = Rendering::StoreOperation::Store;
        depthAttachment.clearValue.depth = 1.0f;
        depthAttachment.clearValue.stencil = 0;

        Rendering::RenderPass renderPass(0, 0, renderTarget.GetWidth(), renderTarget.GetHeight());
        renderPass.AddAttachment(colorAttachment);
        renderPass.AddAttachment(depthAttachment);

        renderPass.SetAttachmentTexture(0, renderTarget.GetColorTexture());
        renderPass.SetAttachmentTexture(1, renderTarget.GetDepthTexture());

        window.resizeEvent.BindMember(&renderPass, &Rendering::RenderPass::Resize);

        Rendering::GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = &device;
        pipelineCreateInfo.renderPass = &renderPass;
        pipelineCreateInfo.pipelineLayout = pipelineLayout;

        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "Position", Format::Vector3 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "Color", Format::Vector4 });

        pipelineCreateInfo.inputAssemblyInfo.topology = PrimitiveTopology::TriangleList;
        pipelineCreateInfo.inputAssemblyInfo.primitiveRestartEnable = false;

        pipelineCreateInfo.rasterizationInfo.cullMode = CullMode::BackFace;
        pipelineCreateInfo.rasterizationInfo.frontFace = FrontFace::CounterClockwise;
        pipelineCreateInfo.rasterizationInfo.discardEnable = false;
        pipelineCreateInfo.rasterizationInfo.polygonMode = PolygonMode::Fill;
        pipelineCreateInfo.rasterizationInfo.depthClampEnable = false;
        pipelineCreateInfo.rasterizationInfo.depthBiasEnable = false;

        pipelineCreateInfo.colorBlendInfo.colorBlendEnable = false;

        pipelineCreateInfo.depthStencilInfo.depthTestEnable = false;
        pipelineCreateInfo.depthStencilInfo.depthWriteEnable = false;
        pipelineCreateInfo.depthStencilInfo.stencilTestEnable = false;
        pipelineCreateInfo.depthStencilInfo.depthCompareOp = CompareOperation::Less;

        pipelineCreateInfo.multisampleInfo.alphaToOneEnable = false;
        pipelineCreateInfo.multisampleInfo.sampleShadingEnable = false;
        pipelineCreateInfo.multisampleInfo.alphaToCoverageEnable = false;
        pipelineCreateInfo.multisampleInfo.sampleCount = SAMPLE_COUNT;

        pipelineCreateInfo.viewportInfo.x = 0;
        pipelineCreateInfo.viewportInfo.y = 0;
        pipelineCreateInfo.viewportInfo.width = window.GetWidth();
        pipelineCreateInfo.viewportInfo.height = window.GetHeight();

        pipelineCreateInfo.scissorInfo.x = 0;
        pipelineCreateInfo.scissorInfo.y = 0;
        pipelineCreateInfo.scissorInfo.width = window.GetWidth();
        pipelineCreateInfo.scissorInfo.height = window.GetHeight();

        const VkPipelineShaderStageCreateInfo shaderStages[] { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };
        pipelineCreateInfo.shaderStages = shaderStages;
        pipelineCreateInfo.shaderStagesCount = 2;

        Vulkan::GraphicsPipeline pipeline;
        if (!pipeline.Initialize(pipelineCreateInfo))
            return EXIT_FAILURE;

        struct Vertex
        {
            Vector3 position;
            Vector4 color;
        };

        const Vertex vertices[]
        {
            Vertex(Vector3(-0.5f, -0.5f, 0.0f), Color::Red),
            Vertex(Vector3(+0.0f, +0.5f, 0.0f), Color::Green),
            Vertex(Vector3(+0.5f, -0.5f, 0.0f), Color::Blue),
        };

        const uint32_t indices[] { 0, 2, 1 };

        Vulkan::Buffer vertexBuffer = CreateVertexBuffer(device, vertices, sizeof(vertices));
        Vulkan::Buffer indexBuffer = CreateIndexBuffer(device, indices, sizeof(indices));

        Scene scene("My Scene");
        EntityHandle cameraEntity = scene.CreateEntity("Camera");
        Camera* camera = cameraEntity->AddComponent<Camera>();
        Transform* cameraTransform = cameraEntity->GetComponent<Transform>();
        cameraTransform->SetPosition(Vector3(0.0f, 0.0f, 1.0f));
        camera->SetDimensions(renderTarget.GetWidth(), renderTarget.GetHeight());
        camera->SetClipPlanes(0.001f, 1000.0f);
        camera->SetProjectionMode(CameraProjectionMode::Orthographic);
        camera->SetOrthographicSize(300.0f);

        EntityHandle triangleEntity = scene.CreateEntity("Triangle");
        scene.OnInit();

        const auto drawTransform = [](const StringView name, Transform* transform)
        {
            ImGui::PushID((const char*)transform->GetUuid().GetValues(), (const char*)(transform->GetUuid().GetValues() + 1));
            if (ImGui::TreeNode(*name))
            {
                Vector3 position = transform->GetPosition();
                if ((ImGui::DragFloat3("Position", position.ValuePtr(), 0.01f, 0, 0, "%.2f")))
                    transform->SetPosition(position);

                Vector3 eulerAngles = transform->GetRotation().ToEulerDegrees();
                if ((ImGui::DragFloat3("Rotation", eulerAngles.ValuePtr(), 0.01f, 0, 0, "%.2f")))
                    transform->SetRotation(Quaternion::FromEulerDegrees(eulerAngles));

                Vector3 scale = transform->GetScale();
                if ((ImGui::DragFloat3("Scale", scale.ValuePtr(), 0.01f, 0, 0, "%.2f")))
                    transform->SetScale(scale);

                ImGui::TreePop();
            }
            ImGui::PopID();
        };


        float lastTime = 0.0;
        while (g_Running)
        {
            float currentTime = (float)Time::Get();
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            window.PollEvents();
            scene.OnUpdate(deltaTime);

            Transform* transform = triangleEntity->GetComponent<Transform>();
            transform->Rotate(Quaternion::FromEulerDegrees(0.0f, 0.0f, 5.0f * deltaTime));
            const Matrix4& viewProj = camera->GetViewProjectionMatrix();
            const Matrix4 mvp = viewProj * transform->GetWorldSpaceMatrix();

            if (device.BeginFrame())
            {
                imgui.BeginFrame();
                ImGui::ShowDemoWindow();
                if (ImGui::Begin("Settings"))
                {
                    drawTransform("Camera", cameraTransform);
                    drawTransform("Triangle", transform);

                    float orthoSize = camera->GetOrthographicSize();
                    if ((ImGui::SliderFloat("Ortho Size", &orthoSize, -300, 300, "%.1f")))
                        camera->SetOrthographicSize(orthoSize);


                }
                ImGui::End();

                ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);


                ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoNav |
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_NoInputs |
                    ImGuiWindowFlags_AlwaysAutoResize;

                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
                if (ImGui::Begin("FPS counter", nullptr, flags))
                {
                    static char str[9] = { 0 };
                    static float timer = 0.0f;
                    timer += deltaTime;
                    if (timer > 0.5f)
                    {
                        const uint32_t fps = 1.0f / deltaTime;
                        std::format_to(str, "{:04} fps", fps);
                        timer = 0.0f;
                    }
                    ImGui::Text(str);
                }
                ImGui::End();
                ImGui::PopStyleVar();
                imgui.EndFrame();


                renderPass.SetAttachmentTexture(0, renderTarget.GetColorTexture());
                renderPass.SetAttachmentTexture(1, renderTarget.GetDepthTexture());
                renderPass.SetAttachmentResolveTexture(0, device.GetSwapchain()->GetCurrentTexture());

                imgui.BeginFrame();
                static bool opened = true;
                ImGui::ShowDemoWindow(&opened);
                imgui.EndFrame();

                Vulkan::CommandBuffer& cmdBuffer = device.GetCurrentCommandBuffer();
                cmdBuffer.BeginRenderPass(renderPass);

                cmdBuffer.ClearColor(0x030303FF, 0);
                cmdBuffer.SetViewport(0, 0, renderTarget.GetWidth(), renderTarget.GetHeight(), 0.0f, 1.0f);
                cmdBuffer.SetScissor(0, 0, renderTarget.GetWidth(), renderTarget.GetHeight());
                cmdBuffer.BindGraphicsPipeline(pipeline);
                cmdBuffer.BindVertexBuffer(vertexBuffer, 0);
                cmdBuffer.BindIndexBuffer(indexBuffer, 0, Format::Uint32);
                cmdBuffer.PushConstants(ShaderStageFlagBits::Vertex, 0, sizeof(Matrix4), &mvp, pipelineLayout);
                cmdBuffer.DrawIndexed(3, 0);

                imgui.Render(cmdBuffer);
                cmdBuffer.EndRenderPass();

                device.EndFrame();
                device.Present();
            }
        }


        device.WaitIdle();
        imgui.Destroy();
        vkDestroyPipelineLayout(device.GetHandle(), pipelineLayout, nullptr);
        pipeline.Destroy();
        fragShaderModule.Destroy();
        vertShaderModule.Destroy();
        vertexBuffer.Destroy();
        indexBuffer.Destroy();
        renderTarget.Destroy();
        device.Destroy();
        window.Destroy();
        return 0;
    }
}
