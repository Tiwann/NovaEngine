#include "Runtime/EntryPoint.h"
#include "Runtime/DesktopWindow.h"
#include "Rendering/Vulkan/Device.h"
#include "Rendering/Vulkan/RenderTarget.h"
#include "Containers/StringView.h"
#include "Rendering/ShaderModule.h"
#include "Containers/StringFormat.h"
#include "Rendering/Vulkan/Conversions.h"
#include "Runtime/Time.h"

#include <vulkan/vulkan.h>
#include <cstdlib>
#include <chrono>

#include "ShaderUtils.h"
#include "Rendering/ComputePipeline.h"
#include "Rendering/DescriptorPool.h"
#include "Rendering/Texture.h"
#include "Rendering/Vulkan/ComputePipeline.h"
#include "Rendering/Vulkan/DescriptorPool.h"
#include "Rendering/Vulkan/ShaderModule.h"
#include "Rendering/Vulkan/Texture.h"
#include "Serialization/FileStream.h"


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
        windowCreateInfo.title = "Hello Compute";
        windowCreateInfo.width = 600;
        windowCreateInfo.height = 400;
        windowCreateInfo.show = true;

        DesktopWindow window;
        window.Initialize(windowCreateInfo);

        Rendering::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.applicationName = "Hello Compute";
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

        Rendering::TextureCreateInfo texCreateInfo;
        texCreateInfo.device = &device;
        texCreateInfo.usageFlags = Rendering::TextureUsageFlagBits::Storage;
        texCreateInfo.width = window.GetWidth();
        texCreateInfo.height = window.GetHeight();
        texCreateInfo.format = Format::R32G32B32A32_FLOAT;
        texCreateInfo.mips = 1;
        texCreateInfo.sampleCount = 1;
        texCreateInfo.data = nullptr;
        texCreateInfo.dataSize = 0;
        Vulkan::Texture texture;
        texture.Initialize(texCreateInfo);

#if 1
        FileStream stream("HelloCompute.slang.spv", OpenModeFlagBits::ReadBinary);
        if (!stream.IsOpened())
            return EXIT_FAILURE;

        size_t fileSize = stream.GetSize();
        Array<uint32_t> computeSpirv(fileSize / sizeof(uint32_t));
        stream.Read(computeSpirv.Data(), fileSize);
#else

        Array<uint32_t> computeSpirv;
        CompileShaderToSpirv(GetAssetPath("Shaders/HelloCompute.slang"), "HelloCompute", computeSpirv);
        FileStream stream("HelloCompute.slang.spirv", OpenModeFlagBits::WriteBinary);
        stream.Write(computeSpirv.Data(), computeSpirv.Size());
        stream.Close();
#endif
        Vulkan::ShaderModule shaderModule = Rendering::ShaderModule::Create<Vulkan::ShaderModule>(device, ShaderStageFlagBits::Compute, computeSpirv);


        VkDescriptorSetLayoutBinding binding;
        binding.binding = 0;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        binding.descriptorCount = 1;
        binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
        binding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo dslCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        dslCreateInfo.bindingCount = 1;
        dslCreateInfo.pBindings = &binding;


        VkDescriptorSetLayout descriptorSetLayout = nullptr;
        VkResult result = vkCreateDescriptorSetLayout(device.GetHandle(), &dslCreateInfo, nullptr, &descriptorSetLayout);
        if (result != VK_SUCCESS)
            return EXIT_FAILURE;

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;

        VkPipelineLayout pipelineLayout;
        result = vkCreatePipelineLayout(device.GetHandle(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
        if (result != VK_SUCCESS)
            return EXIT_FAILURE;

        Rendering::ComputePipelineCreateInfo cpCreateInfo;
        cpCreateInfo.device = &device;
        cpCreateInfo.shaderModule = &shaderModule;
        cpCreateInfo.pipelineLayout = pipelineLayout;
        Vulkan::ComputePipeline computePipeline;
        computePipeline.Initialize(cpCreateInfo);

        Rendering::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
        descriptorPoolCreateInfo.device = &device;
        descriptorPoolCreateInfo.sizes[DescriptorType::StorageImage] = 1;
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
        imageInfo.sampler = nullptr;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.imageView = texture.GetImageView();

        VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descriptorWrite.dstSet = descriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device.GetHandle(), 1, &descriptorWrite, 0, nullptr);

        double lastTime = 0.0f;
        while (g_Running)
        {
            const double currentTime = Time::Get();
            const double deltaTime = currentTime - lastTime;
            lastTime = currentTime;
            static double timer = 0.0f;
            timer += deltaTime;
            if (timer > 1.0)
            {
                const uint32_t fps = 1.0 / deltaTime;
                std::println(std::cout, "Frame Time: {:.2f}ms | FPS: {}", deltaTime * 1000.0, fps);
                timer = 0.0f;
            }

            window.PollEvents();

            if (device.BeginFrame())
            {
                Vulkan::CommandBuffer& commandBuffer = device.GetCurrentCommandBuffer();
                commandBuffer.BindComputePipeline(computePipeline);
                vkCmdBindDescriptorSets(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
                constexpr uint32_t workGroupSizeX = 16;
                constexpr uint32_t workGroupSizeY = 16;

                const uint32_t numGroupsX = (texture.GetWidth() + workGroupSizeX - 1) / workGroupSizeX;
                const uint32_t numGroupsY = (texture.GetHeight() + workGroupSizeY - 1) / workGroupSizeY;

                commandBuffer.Dispatch(numGroupsX, numGroupsY, 1);

                device.BlitToSwapchain(texture, Filter::Nearest);
                device.EndFrame();
                device.Present();
            }
        }


        device.WaitIdle();
        vkDestroyPipelineLayout(device.GetHandle(), pipelineLayout, nullptr);
        renderTarget.Destroy();
        device.Destroy();
        window.Destroy();
        return 0;
    }
}
