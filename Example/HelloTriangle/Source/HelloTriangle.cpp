#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Game/Scene.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/RenderPass.h"
#include "Rendering/Vulkan/Buffer.h"
#include "Rendering/Vulkan/GraphicsPipeline.h"
#include "Rendering/Vulkan/RenderTarget.h"
#include "Rendering/Vulkan/ShaderModule.h"
#include "Runtime/Application.h"
#include "Runtime/ShaderCompiler.h"
#include "Utils/BufferUtils.h"

#include <imgui.h>
#include <vulkan/vulkan.h>

#include "Assets/Sprite.h"
#include "Components/Rendering/SpriteRenderer.h"
#include "Runtime/Path.h"

#define STB_IMAGE_IMPLEMENTATION
#include "External/stb_image.h"

using namespace Nova;

class HelloTriangle : public Application
{

public:
    void OnInit() override
    {
        Window* window = GetWindow();
        Vulkan::Device* device = GetDevice();

        const String shaderPath = Path::Combine(APPLICATION_DIR, "Shaders", "HelloTriangle.slang");
        ShaderCompiler compiler;
        compiler.AddEntryPoint({ "vert", ShaderStageFlagBits::Vertex });
        compiler.AddEntryPoint({ "frag", ShaderStageFlagBits::Fragment });
        compiler.Initialize();
        compiler.Compile(shaderPath, "HelloTriangle", ShaderTarget::SPIRV);

        const Array<uint32_t> vertSpirv = compiler.GetEntryPointBinary(ShaderStageFlagBits::Vertex);
        const Array<uint32_t> fragSpirv = compiler.GetEntryPointBinary(ShaderStageFlagBits::Fragment);
        if (vertSpirv.IsEmpty() || fragSpirv.IsEmpty()) return;

        Vulkan::ShaderModule vertShaderModule = Rendering::ShaderModule::Create<Vulkan::ShaderModule>(*device, ShaderStageFlagBits::Vertex, vertSpirv);
        Vulkan::ShaderModule fragShaderModule = Rendering::ShaderModule::Create<Vulkan::ShaderModule>(*device, ShaderStageFlagBits::Fragment, fragSpirv);

        VkPushConstantRange range;
        range.offset = 0;
        range.size = sizeof(Matrix4);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = &range;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;

        VkResult result = vkCreatePipelineLayout(device->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout);
        if (result != VK_SUCCESS) return;


        Rendering::GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = device;
        pipelineCreateInfo.renderPass = GetRenderPass();
        pipelineCreateInfo.pipelineLayout = m_PipelineLayout;

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
        pipelineCreateInfo.multisampleInfo.sampleCount = 8;

        pipelineCreateInfo.viewportInfo.x = 0;
        pipelineCreateInfo.viewportInfo.y = 0;
        pipelineCreateInfo.viewportInfo.width = GetRenderTarget()->GetWidth();
        pipelineCreateInfo.viewportInfo.height = GetRenderTarget()->GetHeight();

        pipelineCreateInfo.scissorInfo.x = 0;
        pipelineCreateInfo.scissorInfo.y = 0;
        pipelineCreateInfo.scissorInfo.width = GetRenderTarget()->GetWidth();
        pipelineCreateInfo.scissorInfo.height = GetRenderTarget()->GetHeight();

        const VkPipelineShaderStageCreateInfo shaderStages[] { vertShaderModule.GetShaderStageCreateInfo(), fragShaderModule.GetShaderStageCreateInfo() };
        pipelineCreateInfo.shaderStages = shaderStages;
        pipelineCreateInfo.shaderStagesCount = 2;
        m_Pipeline.Initialize(pipelineCreateInfo);

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

        m_VertexBuffer = CreateVertexBuffer(*device, vertices, sizeof(vertices));
        m_IndexBuffer = CreateIndexBuffer(*device, indices, sizeof(indices));

        const auto createScene = [this]() -> Scene*
        {
            Scene* scene = new Scene(this, "Hello Triangle");
            Vulkan::RenderTarget* renderTarget = GetRenderTarget();

            const EntityHandle cameraEntity = scene->CreateEntity("Camera");
            m_Camera = cameraEntity->AddComponent<Camera>();
            m_CameraTransform = cameraEntity->GetComponent<Transform>();
            m_CameraTransform->SetPosition(Vector3(0.0f, 0.0f, 1.0f));
            m_Camera->SetDimensions(renderTarget->GetWidth(), renderTarget->GetHeight());
            m_Camera->SetClipPlanes(0.001f, 1000.0f);
            m_Camera->SetProjectionMode(CameraProjectionMode::Orthographic);
            m_Camera->SetOrthographicSize(300.0f);

            const EntityHandle triangleEntity = scene->CreateEntity("Triangle");
            m_TriangleTransform = triangleEntity->GetComponent<Transform>();


            const StringView texturePath = R"(D:\Dev\HydroRenderer\HydroGame\Assets\Textures\Characters\MaskDude\Fall32.png)";
            stbi_set_flip_vertically_on_load(true);

            int32_t width = 0, height = 0;
            stbi_uc* pixels = stbi_load(*texturePath, &width, &height, nullptr, STBI_rgb_alpha);

            Rendering::TextureCreateInfo createInfo;
            createInfo.device = GetDevice();
            createInfo.usageFlags = Rendering::TextureUsageFlagBits::Sampled;
            createInfo.width = width;
            createInfo.height = height;
            createInfo.format = Format::R8G8B8A8_UNORM;
            createInfo.mips = 1;
            createInfo.data = pixels;
            createInfo.dataSize = width * height * 4;
            createInfo.sampleCount = 1;

            static Vulkan::Texture texture;
            texture.Initialize(createInfo);

            static Sprite sprite;
            sprite.x = 0;
            sprite.y = 0;
            sprite.width = width;
            sprite.height = height;
            sprite.texture = &texture;


            const EntityHandle spriteEntity = scene->CreateEntity("Sprite");
            m_SpriteTransform = spriteEntity->GetComponent<Transform>();
            m_SpriteRenderer = spriteEntity->AddComponent<SpriteRenderer>();
            m_SpriteRenderer->SetSprite(sprite);
            return scene;
        };

        SceneManager* sceneManager = GetSceneManager();
        sceneManager->LoadScene(createScene());
    }

    void OnUpdate(const float deltaTime) override
    {
        m_TriangleTransform->Rotate(Quaternion::FromAxisAngleDegrees(Vector3::Forward, 5.0f * deltaTime));
    }

    void OnGUI() override
    {
        const auto drawTransform = [](const StringView name, Transform* transform)
        {
            ImGui::PushID(transform);
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

        const auto drawSpriteRenderer = [](const StringView name, SpriteRenderer* renderer)
        {
            ImGui::PushID(renderer);
            if (ImGui::TreeNode(*name))
            {

                Vector2 tiling = renderer->GetTiling();
                if (ImGui::DragFloat2("Tiling", tiling.ValuePtr(), 0.01f, 0, 0, "%.2f"))
                    renderer->SetTiling(tiling);

                const StringView flagNames[] = { "Tile With Scale", "Flip Horizontal", "Flip Vertical" };

                if(ImGui::BeginCombo("Flags", nullptr, ImGuiComboFlags_NoPreview))
                {
                    SpriteRendererFlags flags = renderer->GetFlags();
                    for(size_t i = 0; i < std::size(flagNames); i++)
                    {
                        bool selected = flags.Contains(SpriteRendererFlagBits(1 << i));
                        if(ImGui::Selectable(flagNames[i], &selected))
                        {
                            flags.Toggle(SpriteRendererFlagBits(1 << i));
                            renderer->SetFlags(flags);
                        }
                    }
                    ImGui::EndCombo();
                }

                Color colorTint = renderer->GetColorTint();
                if (ImGui::ColorEdit4("Color Tint", (float*)&colorTint, ImGuiColorEditFlags_DisplayHex))
                    renderer->SetColorTint(colorTint);

                uint32_t pixelsPerUnit = renderer->GetPixelsPerUnit();
                if (ImGui::DragInt("Pixels Per Unit", (int*)&pixelsPerUnit, 1, 0, 0))
                    renderer->SetPixelsPerUnit(pixelsPerUnit);

                ImGui::TreePop();
            }
            ImGui::PopID();
        };

        if (ImGui::Begin("Settings"))
        {
            drawTransform("Camera", m_CameraTransform);
            drawTransform("Triangle", m_TriangleTransform);
            drawTransform("Sprite", m_SpriteTransform);
            drawSpriteRenderer("Sprite Renderer", m_SpriteRenderer);
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);


        constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoInputs |
            ImGuiWindowFlags_AlwaysAutoResize;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
        if (ImGui::Begin("FPS counter", nullptr, flags))
        {
            static char str[9] = "0000 fps";
            static float timer = 0.0f;
            const float deltaTime = GetDeltaTime();
            timer += deltaTime;
            if (timer > 0.5f)
            {
                const uint32_t fps = 1.0f / deltaTime;
                std::format_to(str, "{:04} fps", fps);
                timer = 0.0f;
            }
            ImGui::TextUnformatted(str, str + 9);
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void OnRender(Vulkan::CommandBuffer& cmdBuffer) override
    {
        const Matrix4& viewProj = m_Camera->GetViewProjectionMatrix();
        const Matrix4 mvp = viewProj * m_TriangleTransform->GetWorldSpaceMatrix();

        const Vulkan::RenderTarget* renderTarget = GetRenderTarget();
        cmdBuffer.SetViewport(0, 0, renderTarget->GetWidth(), renderTarget->GetHeight(), 0.0f, 1.0f);
        cmdBuffer.SetScissor(0, 0, renderTarget->GetWidth(), renderTarget->GetHeight());
        cmdBuffer.BindGraphicsPipeline(m_Pipeline);
        cmdBuffer.BindVertexBuffer(m_VertexBuffer, 0);
        cmdBuffer.BindIndexBuffer(m_IndexBuffer, 0, Format::Uint32);
        cmdBuffer.PushConstants(ShaderStageFlagBits::Vertex, 0, sizeof(Matrix4), &mvp, m_PipelineLayout);
        cmdBuffer.DrawIndexed(3, 0);
    }

    void OnDestroy() override
    {

    }

private:
    Vulkan::GraphicsPipeline m_Pipeline;
    Vulkan::Buffer m_VertexBuffer;
    Vulkan::Buffer m_IndexBuffer;
    VkPipelineLayout m_PipelineLayout = nullptr;

    Camera* m_Camera = nullptr;
    Transform* m_CameraTransform = nullptr;
    Transform* m_TriangleTransform = nullptr;
    Transform* m_SpriteTransform = nullptr;
    SpriteRenderer* m_SpriteRenderer = nullptr;
};

namespace Nova
{
    extern "C" Application* CreateApplication()
    {
        return new HelloTriangle();
    }
}