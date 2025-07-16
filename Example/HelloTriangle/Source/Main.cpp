#include "Runtime/EntryPoint.h"
#include "Runtime/DesktopWindow.h"
#include "Audio/AudioSystem.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/RenderTarget.h"
#include "Rendering/GraphicsPipeline.h"
#include "Rendering/Vulkan/GraphicsPipeline.h"
#include "Rendering/Vulkan/Texture.h"

#include <cstdlib>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include <External/stb_image.h>

#include "Containers/StringView.h"


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
        if (!window.Initialize(windowCreateInfo))
            return EXIT_FAILURE;

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
        renderTargetCreateInfo.sampleCount = 8;

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

        /*Rendering::GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = &device;
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "a_Position", Format::Vector3 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "a_TexCoords", Format::Vector2 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "a_Normal", Format::Vector3 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "a_Color", Format::Vector4 });

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
        pipelineCreateInfo.viewportInfo.width = window.GetWidth();
        pipelineCreateInfo.viewportInfo.height = window.GetHeight();

        pipelineCreateInfo.scissorInfo.x = 0;
        pipelineCreateInfo.scissorInfo.y = 0;
        pipelineCreateInfo.scissorInfo.width = window.GetWidth();
        pipelineCreateInfo.scissorInfo.height = window.GetHeight();

        Vulkan::GraphicsPipeline pipeline;
        if (!pipeline.Initialize(pipelineCreateInfo))
            return EXIT_FAILURE;*/

        const StringView filepath = R"(D:\Dev\HydroRenderer\HydroGame\Assets\Textures\Characters\MaskDude\Jump32.png)";
        stbi_set_flip_vertically_on_load(true);
        int32_t width, height, channels;
        uint8_t* pixels = stbi_load(*filepath, &width, &height, &channels, STBI_rgb_alpha);

        Rendering::TextureCreateInfo textureCreateInfo;
        textureCreateInfo.device = &device;
        textureCreateInfo.format = Format::R8G8B8A8_UNORM;
        textureCreateInfo.width = width;
        textureCreateInfo.height = height;
        textureCreateInfo.mips = 1;
        textureCreateInfo.sampleCount = 1;
        textureCreateInfo.data = pixels;
        textureCreateInfo.dataSize = width * height * 4 * sizeof(uint8_t);

        Vulkan::Texture texture;
        if (!texture.Initialize(textureCreateInfo))
            return EXIT_FAILURE;

        while (g_Running)
        {
            window.PollEvents();

            if (device.BeginFrame())
            {
                Vulkan::CommandBuffer& commandBuffer = device.GetCurrentCommandBuffer();
                renderTarget.BeginRendering(commandBuffer);
                renderTarget.Clear(Color::Blue);
                renderTarget.EndRendering();

                device.ResolveRenderTarget(renderTarget);
                device.EndFrame();
                device.Present();
            }
        }

        texture.Destroy();
        renderTarget.Destroy();
        device.Destroy();
        window.Destroy();
        return 0;
    }
}
