#include "Renderer2D.h"
#include "Device.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Assets/Sprite.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "DescriptorPool.h"
#include "Components/Camera.h"
#include "Containers/StringFormat.h"
#include "Vulkan/Device.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/Sampler.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/CommandBuffer.h"
#include "Vulkan/ShaderModule.h"
#include "Vulkan/DescriptorPool.h"
#include "Vulkan/Texture.h"
#include "External/ShaderUtils.h"

#include <cstdint>
#include <vulkan/vulkan.h>

namespace Nova::Rendering
{
    struct QuadVertex
    {
        Vector3 position;
        Vector2 texCoords;
        Vector4 color;
        uint32_t textureIndex;
    };

    static uint32_t QuadCount = 0;
    static Array<QuadVertex> QuadVertices;
    static Array<uint32_t> QuadIndices;
    static Array<Sprite> Sprites;

    struct QuadPipeline
    {
        Vulkan::Buffer vertexBuffer;
        Vulkan::Buffer indexBuffer;
        Vulkan::Sampler sampler;

        Vulkan::ShaderModule vertexModule;
        Vulkan::ShaderModule fragmentModule;
        Vulkan::GraphicsPipeline pipelineState;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSet descriptorSet;
        VkPipelineLayout pipelineLayout;

        bool Initialize(Device& device, RenderTarget& renderTarget, uint32_t maxQuads)
        {
            BufferCreateInfo bufferCreateInfo;
            bufferCreateInfo.device = &device;
            bufferCreateInfo.usage = BufferUsage::VertexBuffer;
            bufferCreateInfo.size = maxQuads * 4 * sizeof(QuadVertex);
            if (!vertexBuffer.Initialize(bufferCreateInfo))
                return false;


            bufferCreateInfo.usage = BufferUsage::IndexBuffer;
            bufferCreateInfo.size = maxQuads * 6 * sizeof(uint32_t);
            if (!indexBuffer.Initialize(bufferCreateInfo))
                return false;

            SamplerCreateInfo samplerCreateInfo;
            samplerCreateInfo.device = &device;
            if (!sampler.Initialize(samplerCreateInfo))
                return false;



            Array<uint32_t> vertSpirv, fragSpirv;
            CompileShaderToSpirv(StringFormat("Assets/Shaders/Sprite.slang"), "Sprite", vertSpirv, fragSpirv);
            vertexModule = ShaderModule::Create<Vulkan::ShaderModule>(device, ShaderStageFlagBits::Vertex, vertSpirv);
            fragmentModule = ShaderModule::Create<Vulkan::ShaderModule>(device, ShaderStageFlagBits::Fragment, fragSpirv);

            VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertexShaderStageCreateInfo.module = vertexModule.GetHandle();
            vertexShaderStageCreateInfo.pName = "main";

            VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragmentShaderStageCreateInfo.module = fragmentModule.GetHandle();
            fragmentShaderStageCreateInfo.pName = "main";

            VkDescriptorSetLayoutBinding layoutBinding
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = (uint32_t)Sprites.Count(),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            };

            VkDescriptorBindingFlags bindingFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
            VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
            bindingFlagsInfo.bindingCount = 1;
            bindingFlagsInfo.pBindingFlags = &bindingFlags;

            VkDescriptorSetLayoutCreateInfo dslCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
            dslCreateInfo.bindingCount = 1;
            dslCreateInfo.pBindings = &layoutBinding;
            dslCreateInfo.pNext = &bindingFlagsInfo;

            VkResult result = vkCreateDescriptorSetLayout(((Vulkan::Device&)device).GetHandle(), &dslCreateInfo, nullptr, &descriptorSetLayout);
            if (result != VK_SUCCESS)
                return EXIT_FAILURE;

            VkPushConstantRange range;
            range.offset = 0;
            range.size = sizeof(Matrix4);
            range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
            pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
            pipelineLayoutCreateInfo.setLayoutCount = 1;
            pipelineLayoutCreateInfo.pPushConstantRanges = &range;
            pipelineLayoutCreateInfo.pushConstantRangeCount = 1;

            result = vkCreatePipelineLayout(((Vulkan::Device&)device).GetHandle(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
            if (result != VK_SUCCESS)
                return EXIT_FAILURE;


            DescriptorPoolCreateInfo descriptorPoolCreateInfo;
            descriptorPoolCreateInfo.device = &device;
            descriptorPoolCreateInfo.sizes[ResourceBindingType::CombinedTextureSampler] = 32;
            descriptorPoolCreateInfo.maxSets = 1;
            Vulkan::DescriptorPool descriptorPool;
            if (!descriptorPool.Initialize(descriptorPoolCreateInfo))
                return EXIT_FAILURE;

            VkDescriptorSetVariableDescriptorCountAllocateInfo countInfo { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO };
            uint32_t spriteCount = Sprites.Count();
            countInfo.descriptorSetCount = 1;
            countInfo.pDescriptorCounts = &spriteCount;

            VkDescriptorSetAllocateInfo dsAllocateInfo = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
            dsAllocateInfo.descriptorPool = descriptorPool.GetHandle();
            dsAllocateInfo.pSetLayouts = &descriptorSetLayout;
            dsAllocateInfo.descriptorSetCount = 1;
            dsAllocateInfo.pNext = &countInfo;
            vkAllocateDescriptorSets(((Vulkan::Device&)device).GetHandle(), &dsAllocateInfo, &descriptorSet);


            Array<VkWriteDescriptorSet> writeDescriptors;
            for (size_t i = 0; i < Sprites.Count(); i++)
            {
                const Sprite& sprite = Sprites[i];

                VkDescriptorImageInfo imageInfo;
                imageInfo.sampler = sampler.GetHandle();
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = ((Vulkan::Texture*)sprite.texture)->GetImageView();

                VkWriteDescriptorSet write = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                write.dstSet = descriptorSet;
                write.dstBinding = 0;
                write.dstArrayElement = i;
                write.descriptorCount = 1;
                write.pImageInfo = &imageInfo;

                writeDescriptors.Add(write);
            }

            vkUpdateDescriptorSets(((Vulkan::Device&)device).GetHandle(), writeDescriptors.Count(), writeDescriptors.Data(), 0, nullptr);

            GraphicsPipelineCreateInfo pipelineCreateInfo;
            pipelineCreateInfo.device = &device;
            pipelineCreateInfo.pipelineLayout = pipelineLayout;

            pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "POSITION", Format::Vector3 });
            pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "TEXCOORDS", Format::Vector2 });
            pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "COLOR", Format::Vector4 });
            pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "TEXINDEX", Format::Uint32 });

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
            pipelineCreateInfo.multisampleInfo.sampleCount = 1;

            pipelineCreateInfo.viewportInfo.x = 0;
            pipelineCreateInfo.viewportInfo.y = 0;
            pipelineCreateInfo.viewportInfo.width = renderTarget.GetWidth();
            pipelineCreateInfo.viewportInfo.height = renderTarget.GetHeight();

            pipelineCreateInfo.scissorInfo.x = 0;
            pipelineCreateInfo.scissorInfo.y = 0;
            pipelineCreateInfo.scissorInfo.width = renderTarget.GetWidth();
            pipelineCreateInfo.scissorInfo.height = renderTarget.GetHeight();

            const VkPipelineShaderStageCreateInfo shaderStages[2] { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };
            pipelineCreateInfo.shaderStages = shaderStages;
            pipelineCreateInfo.shaderStagesCount = 2;

            if (!pipelineState.Initialize(pipelineCreateInfo))
                return false;

            return true;
        }

        void Destroy()
        {
            vertexBuffer.Destroy();
            indexBuffer.Destroy();
            sampler.Destroy();
            pipelineState.Destroy();
        }
    };

    static QuadPipeline quadPipeline;

    bool Renderer2D::Initialize(Device& device, RenderTarget& renderTarget)
    {
        if (!quadPipeline.Initialize(device, renderTarget, 10000))
            return false;
        return true;
    }

    void Renderer2D::Destroy()
    {
        quadPipeline.Destroy();
        QuadCount = 0;
        QuadVertices.Clear();
        QuadIndices.Clear();
        Sprites.Clear();
    }

    bool Renderer2D::BeginScene(const Camera* camera)
    {
        if (!camera) return false;
        QuadVertices.Clear();
        QuadIndices.Clear();
        Sprites.Clear();
        return true;
    }

    void Renderer2D::EndScene()
    {
        BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.device = m_Device;
        stagingBufferCreateInfo.usage = BufferUsage::StagingBuffer;
        stagingBufferCreateInfo.size = QuadVertices.Size();

        Vulkan::Buffer stagingBuffer;
        stagingBuffer.Initialize(stagingBufferCreateInfo);
        stagingBuffer.CPUCopy(QuadVertices.Data(), 0, QuadVertices.Size());
        stagingBuffer.GPUCopy(quadPipeline.vertexBuffer, 0, 0, QuadVertices.Size());

        stagingBufferCreateInfo.size = QuadIndices.Size();
        stagingBuffer.Initialize(stagingBufferCreateInfo);
        stagingBuffer.CPUCopy(QuadIndices.Data(), 0, QuadIndices.Size());
        stagingBuffer.GPUCopy(quadPipeline.indexBuffer, 0, 0, QuadIndices.Size());
    }

    void Renderer2D::Render()
    {
        Vulkan::Device* device = (Vulkan::Device*)m_Device;
        Vulkan::CommandBuffer& cmdBuff = device->GetCurrentCommandBuffer();
        vkCmdBindDescriptorSets(cmdBuff.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, quadPipeline.pipelineLayout, 0, 1, &quadPipeline.descriptorSet, 0, nullptr);
        cmdBuff.SetViewport(0, 0, m_Camera->GetWidth(), m_Camera->GetHeight(), 0.0f, 1.0f);
        cmdBuff.SetScissor(0, 0, m_Camera->GetWidth(), m_Camera->GetHeight());
        cmdBuff.BindVertexBuffer(quadPipeline.vertexBuffer, 0);
        cmdBuff.BindIndexBuffer(quadPipeline.indexBuffer, 0, Format::Uint32);
        cmdBuff.DrawIndexed(QuadIndices.Count(), 0);
    }

    void Renderer2D::DrawSprite(const Vector3& position, const Sprite& sprite)
    {
        if (!sprite.texture)
            return;

        NOVA_ASSERT(sprite.x < sprite.texture->GetWidth() &&
                   sprite.y < sprite.texture->GetHeight() &&
                   sprite.x + sprite.width <= sprite.texture->GetWidth() &&
                   sprite.y + sprite.height <= sprite.texture->GetHeight(), "Failed to created sprite");

        const float textureWidth = sprite.texture->GetWidth();
        const float textureHeight = sprite.texture->GetHeight();

        const Vector2 uv0 = {(float)sprite.x / textureWidth, ((float)sprite.y + sprite.height) / textureHeight};
        const Vector2 uv1 = {((float)sprite.x + sprite.width) / textureWidth, ((float)sprite.y + sprite.height) / textureHeight};
        const Vector2 uv2 = {((float)sprite.x + sprite.width) / textureWidth, (float)sprite.y / textureHeight};
        const Vector2 uv3 = {(float)sprite.x / textureWidth, (float)sprite.y / textureHeight};

        Sprites.AddUnique(sprite);

        const QuadVertex vertices[]
        {
            { position + Vector3(-0.5f, -0.5f, 0.0f), uv0, Color::White, (uint32_t)Sprites.Find(sprite) },
            { position + Vector3(-0.5f, +0.5f, 0.0f), uv1, Color::White, (uint32_t)Sprites.Find(sprite) },
            { position + Vector3(+0.5f, +0.5f, 0.0f), uv2, Color::White, (uint32_t)Sprites.Find(sprite) },
            { position + Vector3(+0.5f, -0.5f, 0.0f), uv3, Color::White, (uint32_t)Sprites.Find(sprite) },
        };

        const uint32_t indices[6]
        {
            QuadCount * 4 + 0,
            QuadCount * 4 + 2,
            QuadCount * 4 + 1,
            QuadCount * 4 + 0,
            QuadCount * 4 + 3,
            QuadCount * 4 + 2,
        };

        QuadVertices.AddRange(vertices, std::size(vertices));
        QuadIndices.AddRange(indices, std::size(indices));
        QuadCount++;
    }
}
