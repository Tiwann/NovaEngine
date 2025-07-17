#include "Runtime/EntryPoint.h"
#include "Runtime/DesktopWindow.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/RenderTarget.h"
#include "Containers/StringView.h"
#include "Rendering/Vertex.h"
#include "Rendering/Vulkan/Buffer.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/Vulkan/GraphicsPipeline.h"
#include "ShaderUtils.h"
#include "BufferUtils.h"

#include <vulkan/vulkan.h>
#include <cstdlib>
#include <chrono>

static constexpr uint32_t SAMPLE_COUNT = 8;
static bool g_Running = true;

namespace Nova
{
    int GuardedMain(int, char**)
    {
        WindowCreateInfo windowCreateInfo;
        windowCreateInfo.title = "Hello Triangle";
        windowCreateInfo.width = 1600;
        windowCreateInfo.height = 900;
        windowCreateInfo.show = true;

        DesktopWindow window;
        window.Initialize(windowCreateInfo);

        Rendering::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.applicationName = "Hello Triangle";
        deviceCreateInfo.versionMajor = 1;
        deviceCreateInfo.versionMinor = 0;
        deviceCreateInfo.window = &window;

        Vulkan::Device device;
        if (!device.Initialize(deviceCreateInfo))
            return EXIT_FAILURE;

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

        window.closeEvent.Bind([] { g_Running = false; });

        window.maximizeEvent.Bind([&device]
        {
            Vulkan::Swapchain* swapchain = device.GetSwapchain();
            swapchain->Invalidate();
        });

        window.resizeEvent.Bind([&device, &renderTarget](const int32_t newX, const int32_t newY)
        {
            Vulkan::Swapchain* swapchain = device.GetSwapchain();
            swapchain->Invalidate();

            renderTarget.Resize(newX, newY);
        });


        Array<uint32_t> vertSpirv, fragSpirv;
        CompileShaderToSpirV(R"(Shaders/HelloTriangle.slang)", vertSpirv, fragSpirv);

        VkShaderModule vertexShaderModule = nullptr, fragmentShaderModule = nullptr;
        VkShaderModuleCreateInfo vertShaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        vertShaderModuleCreateInfo.codeSize = vertSpirv.Size();
        vertShaderModuleCreateInfo.pCode = vertSpirv.Data();
        VkResult result = vkCreateShaderModule(device.GetHandle(), &vertShaderModuleCreateInfo, nullptr, &vertexShaderModule);
        if (result != VK_SUCCESS)
            return EXIT_FAILURE;

        VkShaderModuleCreateInfo fragShaderModuleCreateInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
        fragShaderModuleCreateInfo.codeSize = fragSpirv.Size();
        fragShaderModuleCreateInfo.pCode = fragSpirv.Data();
        result = vkCreateShaderModule(device.GetHandle(), &fragShaderModuleCreateInfo, nullptr, &fragmentShaderModule);
        if (result != VK_SUCCESS)
            return EXIT_FAILURE;

        VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderStageCreateInfo.module = vertexShaderModule;
        vertexShaderStageCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderStageCreateInfo.module = fragmentShaderModule;
        fragmentShaderStageCreateInfo.pName = "main";


        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;

        VkPipelineLayout pipelineLayout;
        result = vkCreatePipelineLayout(device.GetHandle(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
        if (result != VK_SUCCESS)
            return EXIT_FAILURE;


        Rendering::GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = &device;
        pipelineCreateInfo.pipelineLayout = pipelineLayout;

        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "Position", Format::Vector3 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "TexCoords", Format::Vector2 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "Normal", Format::Vector3 });
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

        const Vertex vertices[]
        {
            Vertex(Vector3(-0.5f, -0.5f, 0.0f), Vector2(0.0f, 0.0f), Vector3::Zero, Color::Red),
            Vertex(Vector3(+0.0f, +0.5f, 0.0f), Vector2(0.0f, 1.0f), Vector3::Zero, Color::Green),
            Vertex(Vector3(+0.5f, -0.5f, 0.0f), Vector2(1.0f, 1.0f), Vector3::Zero, Color::Blue),
        };

        const uint32_t indices[] { 0, 2, 1 };

        Vulkan::Buffer vertexBuffer = CreateVertexBuffer(device, vertices, sizeof(vertices));
        Vulkan::Buffer indexBuffer = CreateIndexBuffer(device, indices, sizeof(indices));

        while (g_Running)
        {
            window.PollEvents();

            if (device.BeginFrame())
            {
                Vulkan::CommandBuffer& commandBuffer = device.GetCurrentCommandBuffer();
                renderTarget.BeginRendering(commandBuffer);
                renderTarget.Clear(0x101010FF);
                commandBuffer.SetViewport(0, 0, renderTarget.GetWidth(), renderTarget.GetHeight(), 0.0f, 1.0f);
                commandBuffer.SetScissor(0, 0, renderTarget.GetWidth(), renderTarget.GetHeight());
                commandBuffer.BindGraphicsPipeline(pipeline);
                commandBuffer.BindVertexBuffer(vertexBuffer, 0);
                commandBuffer.BindIndexBuffer(indexBuffer, 0, Format::R32_UINT);
                commandBuffer.DrawIndexed(3, 0);
                renderTarget.EndRendering();

                device.ResolveToSwapchain(renderTarget);
                //device.BlitToSwapchain(renderTarget, Filter::Nearest);
                device.EndFrame();
                device.Present();
            }
        }

        vertexBuffer.Destroy();
        indexBuffer.Destroy();
        renderTarget.Destroy();
        device.Destroy();
        window.Destroy();
        return 0;
    }
}
