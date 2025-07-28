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
#include "ShaderUtils.h"
#include "BufferUtils.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Containers/StringFormat.h"
#include "Rendering/Vulkan/Conversions.h"
#include "Rendering/Vulkan/DescriptorPool.h"
#include "Runtime/Time.h"
#include "Runtime/ScopedTimer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <External/stb_image.h>
#include <vulkan/vulkan.h>
#include <cstdlib>
#include <chrono>

#include "Rendering/Sampler.h"
#include "Rendering/Vulkan/Sampler.h"
#include "Rendering/Vulkan/Texture.h"


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
        windowCreateInfo.width = 600;
        windowCreateInfo.height = 400;
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

        int32_t textureWidth = 0, textureHeight = 0;
        stbi_set_flip_vertically_on_load(true);
        stbi_uc* pixels = stbi_load(*GetAssetPath("Assets/Checker.png"), &textureWidth, &textureHeight, nullptr, STBI_rgb_alpha);

        Rendering::TextureCreateInfo texCreateInfo;
        texCreateInfo.device = &device;
        texCreateInfo.usageFlags = Rendering::TextureUsageFlagBits::Sampled;
        texCreateInfo.format = Format::R8G8B8A8_UNORM;
        texCreateInfo.width = textureWidth;
        texCreateInfo.height = textureHeight;
        texCreateInfo.sampleCount = 1;
        texCreateInfo.mips = 1;
        texCreateInfo.data = pixels;
        texCreateInfo.dataSize = textureWidth * textureHeight * 4 * sizeof(stbi_uc);

        Vulkan::Texture texture;
        texture.Initialize(texCreateInfo);

        Rendering::SamplerCreateInfo samplerCreateInfo;
        samplerCreateInfo.device =  &device;
        Vulkan::Sampler sampler;
        sampler.Initialize(samplerCreateInfo);


        stbi_image_free(pixels);

        Array<uint32_t> vertSpirv, fragSpirv;
        CompileShaderToSpirv(GetAssetPath("Shaders/HelloTexture.slang"), "HelloTexture", vertSpirv, fragSpirv);

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


        VkDescriptorSetLayoutBinding layoutBindings[2]
        {
            VkDescriptorSetLayoutBinding {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            },
            VkDescriptorSetLayoutBinding {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            },
        };

        VkDescriptorSetLayoutCreateInfo dslCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        dslCreateInfo.bindingCount = std::size(layoutBindings);
        dslCreateInfo.pBindings = layoutBindings;


        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkResult result = vkCreateDescriptorSetLayout(device.GetHandle(), &dslCreateInfo, nullptr, &descriptorSetLayout);
        if (result != VK_SUCCESS)
            return EXIT_FAILURE;

        VkPushConstantRange range;
        range.offset = 0;
        range.size = sizeof(float);
        range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = &range;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;

        VkPipelineLayout pipelineLayout;
        result = vkCreatePipelineLayout(device.GetHandle(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
        if (result != VK_SUCCESS)
            return EXIT_FAILURE;


        Rendering::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.device = &device;
        descriptorPoolCreateInfo.sizes[DescriptorType::SampledImage] = 1;
        descriptorPoolCreateInfo.sizes[DescriptorType::Sampler] = 1;
        descriptorPoolCreateInfo.maxSets = 1;
        Vulkan::DescriptorPool descriptorPool;
        if (!descriptorPool.Initialize(descriptorPoolCreateInfo))
            return EXIT_FAILURE;

        VkDescriptorSetAllocateInfo dsAllocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        dsAllocateInfo.descriptorPool = descriptorPool.GetHandle();
        dsAllocateInfo.pSetLayouts = &descriptorSetLayout;
        dsAllocateInfo.descriptorSetCount = 1;
        VkDescriptorSet descriptorSet = nullptr;
        vkAllocateDescriptorSets(device.GetHandle(), &dsAllocateInfo, &descriptorSet);


        VkDescriptorImageInfo imageInfo;
        imageInfo.sampler = sampler.GetHandle();
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture.GetImageView();

        VkWriteDescriptorSet samplerDescriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        samplerDescriptorWrite.dstSet = descriptorSet;
        samplerDescriptorWrite.dstBinding = 0;
        samplerDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        samplerDescriptorWrite.descriptorCount = 1;
        samplerDescriptorWrite.pImageInfo = &imageInfo;

        VkWriteDescriptorSet imageDescriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        imageDescriptorWrite.dstSet = descriptorSet;
        imageDescriptorWrite.dstBinding = 1;
        imageDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        imageDescriptorWrite.descriptorCount = 1;
        imageDescriptorWrite.pImageInfo = &imageInfo;

        const VkWriteDescriptorSet writeDescriptors[2] { samplerDescriptorWrite, imageDescriptorWrite };
        vkUpdateDescriptorSets(device.GetHandle(), std::size(writeDescriptors), writeDescriptors, 0, nullptr);


        Rendering::GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = &device;
        pipelineCreateInfo.pipelineLayout = pipelineLayout;

        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "Position", Format::Vector3 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "TexCoords", Format::Vector2 });

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
        pipelineCreateInfo.renderTarget = &renderTarget;

        const VkPipelineShaderStageCreateInfo shaderStages[] { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };
        pipelineCreateInfo.shaderStages = shaderStages;
        pipelineCreateInfo.shaderStagesCount = 2;

        Vulkan::GraphicsPipeline pipeline;
        if (!pipeline.Initialize(pipelineCreateInfo))
            return EXIT_FAILURE;


        struct Vertex
        {
            Vector3 position;
            Vector2 texCoords;
        };

        const Vertex vertices[]
        {
            { Vector3(-0.5f, -0.5f, 0.0f), Vector2(0.0f, 0.0f) },
            { Vector3(-0.5f, +0.5f, 0.0f), Vector2(0.0f, 1.0f) },
            { Vector3(+0.5f, +0.5f, 0.0f), Vector2(1.0f, 1.0f) },
            { Vector3(+0.5f, -0.5f, 0.0f), Vector2(1.0f, 0.0f) }
        };

        const uint32_t indices[] { 0, 2, 1, 0, 3, 2 };

        Vulkan::Buffer vertexBuffer = CreateVertexBuffer(device, vertices, sizeof(vertices));
        Vulkan::Buffer indexBuffer = CreateIndexBuffer(device, indices, sizeof(indices));


        const auto onTimer = [](double deltaTime)
        {
            static double timer = 0.0f;
            timer += deltaTime;
            if (timer > 1.0)
            {
                const uint32_t fps = 1.0 / deltaTime;
                std::println(std::cout, "Frame Time: {:.2f}ms | FPS: {}", deltaTime * 1000.0, fps);
                timer = 0.0f;
            }
        };

        while (g_Running)
        {
            const ScopedTimer timer(onTimer);

            window.PollEvents();

            if (device.BeginFrame())
            {
                Vulkan::CommandBuffer& commandBuffer = device.GetCurrentCommandBuffer();
                renderTarget.BeginRendering(commandBuffer);
                renderTarget.Clear(0x060606FF);
                commandBuffer.SetViewport(0, 0, renderTarget.GetWidth(), renderTarget.GetHeight(), 0.0f, 1.0f);
                commandBuffer.SetScissor(0, 0, renderTarget.GetWidth(), renderTarget.GetHeight());
                commandBuffer.BindGraphicsPipeline(pipeline);
                commandBuffer.BindVertexBuffer(vertexBuffer, 0);
                commandBuffer.BindIndexBuffer(indexBuffer, 0, Format::Uint32);
                vkCmdBindDescriptorSets(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
                commandBuffer.DrawIndexed(6, 0);
                renderTarget.EndRendering();

                if constexpr (SAMPLE_COUNT > 1)
                {
                    device.ResolveToSwapchain(renderTarget);
                } else {
                    device.BlitToSwapchain(renderTarget, Filter::Nearest);
                }
                device.EndFrame();
                device.Present();
            }
        }


        device.WaitIdle();
        vkFreeDescriptorSets(device.GetHandle(), descriptorPool.GetHandle(), 1, &descriptorSet);
        descriptorPool.Destroy();
        sampler.Destroy();
        texture.Destroy();
        vkDestroyDescriptorSetLayout(device.GetHandle(), descriptorSetLayout, nullptr);
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
