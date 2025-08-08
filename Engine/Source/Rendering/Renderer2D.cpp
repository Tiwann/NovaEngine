#include "Renderer2D.h"

#include "Texture.h"
#include "Assets/Sprite.h"
#include "Containers/Array.h"
#include "Math/Matrix2.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "Vulkan/Buffer.h"
#include "Vulkan/CommandBuffer.h"
#include "Vulkan/GraphicsPipeline.h"
#include "Vulkan/Sampler.h"
#include <vulkan/vulkan.h>

#include "RenderPass.h"
#include "Math/Matrix4.h"
#include "Runtime/Path.h"
#include "Runtime/ShaderCompiler.h"
#include "Utils/BufferUtils.h"
#include "Vulkan/DescriptorPool.h"
#include "Vulkan/Device.h"
#include "Vulkan/ShaderBindingSetLayout.h"
#include "Vulkan/ShaderModule.h"


namespace Nova
{
    struct QuadVertex
    {
        Vector2 position;
        Vector2 texCoord;
        uint32_t spriteId;
    };

    static struct QuadBatch
    {
        struct alignas(16) Uniform
        {
            Matrix4 transform;
            Vector2 tiling;
            Vector2 __padding;
            Matrix2 spriteScale;
            Vector4 color;
        };

        Array<QuadVertex> vertices;
        Array<uint32_t> indices;
        Array<Sprite> sprites;
        Array<Uniform> uniforms;
        uint32_t quadCount = 0;

        Vulkan::Sampler sampler;
        Vulkan::Buffer uniformBuffer;
        Vulkan::Buffer uboStaging;
        Vulkan::GraphicsPipeline pipeline;
        VkPipelineLayout pipelineLayout;
        Vulkan::ShaderBindingSetLayout bindingSetLayout;
        VkDescriptorSet descriptorSet;

        Vulkan::Buffer vertexBuffer;
        Vulkan::Buffer indexBuffer;
    } s_QuadData;


    bool Renderer2D::Initialize(const Renderer2DCreateInfo& createInfo)
    {
        m_Device = createInfo.device;
        Rendering::SamplerCreateInfo samplerInfo;
        samplerInfo.device = createInfo.device;
        if (!s_QuadData.sampler.Initialize(samplerInfo))
            return false;

        Rendering::BufferCreateInfo uboCreateInfo;
        uboCreateInfo.device = createInfo.device;
        uboCreateInfo.size = createInfo.maxQuads * sizeof(QuadBatch::Uniform);
        uboCreateInfo.usage = Rendering::BufferUsage::UniformBuffer;
        if (!s_QuadData.uniformBuffer.Initialize(uboCreateInfo))
            return false;

        Rendering::BufferCreateInfo stagingBufferCreateInfo;
        stagingBufferCreateInfo.device = createInfo.device;
        stagingBufferCreateInfo.size = createInfo.maxQuads * sizeof(QuadBatch::Uniform);
        stagingBufferCreateInfo.usage = Rendering::BufferUsage::StagingBuffer;
        if (!s_QuadData.uboStaging.Initialize(uboCreateInfo))
            return false;


        const String shaderPath = Path::Combine(NOVA_ENGINE_ROOT_DIR, "Engine/Assets/Shaders/Sprite.slang");
        ShaderCompiler compiler;
        compiler.AddEntryPoint({ "vert", ShaderStageFlagBits::Vertex });
        compiler.AddEntryPoint({ "frag", ShaderStageFlagBits::Fragment });
        compiler.Initialize();
        compiler.Compile(shaderPath, "HelloTriangle", ShaderTarget::SPIRV);

        const Array<uint32_t> vertSpirv = compiler.GetEntryPointBinary(ShaderStageFlagBits::Vertex);
        const Array<uint32_t> fragSpirv = compiler.GetEntryPointBinary(ShaderStageFlagBits::Fragment);
        if (vertSpirv.IsEmpty() || fragSpirv.IsEmpty()) return false;

        Vulkan::ShaderModule vertShaderModule = Rendering::ShaderModule::Create<Vulkan::ShaderModule>(*createInfo.device, ShaderStageFlagBits::Vertex, vertSpirv);
        Vulkan::ShaderModule fragShaderModule = Rendering::ShaderModule::Create<Vulkan::ShaderModule>(*createInfo.device, ShaderStageFlagBits::Fragment, fragSpirv);

        s_QuadData.bindingSetLayout.SetBinding(0, { "uniforms", ShaderStageFlagBits::Fragment, ResourceBindingType::UniformBuffer, 1 });
        s_QuadData.bindingSetLayout.SetBinding(0, { "textures", ShaderStageFlagBits::Fragment, ResourceBindingType::CombinedTextureSampler });
        s_QuadData.bindingSetLayout.Build();

        Vulkan::Device* device = (Vulkan::Device*)createInfo.device;
        VkDescriptorSetLayout setLayout = s_QuadData.bindingSetLayout.GetDescriptorSetLayout();

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        pipelineLayoutCreateInfo.pSetLayouts = &setLayout;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        VkResult result = vkCreatePipelineLayout(device->GetHandle(), &pipelineLayoutCreateInfo, nullptr, &s_QuadData.pipelineLayout);
        if (result != VK_SUCCESS) return false;


        Rendering::GraphicsPipelineCreateInfo pipelineCreateInfo;
        pipelineCreateInfo.device = createInfo.device;
        pipelineCreateInfo.renderPass = createInfo.renderPass;
        pipelineCreateInfo.pipelineLayout = s_QuadData.pipelineLayout;

        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "Position", Format::Vector2 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "TexCoords", Format::Vector2 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "SpriteId", Format::Uint32 });
        pipelineCreateInfo.vertexInputInfo.layout.AddAttribute({ "QuadId", Format::Uint32 });

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
        pipelineCreateInfo.viewportInfo.width = createInfo.renderPass->GetWidth();
        pipelineCreateInfo.viewportInfo.height = createInfo.renderPass->GetHeight();

        pipelineCreateInfo.scissorInfo.x = 0;
        pipelineCreateInfo.scissorInfo.y = 0;
        pipelineCreateInfo.scissorInfo.width = createInfo.renderPass->GetWidth();
        pipelineCreateInfo.scissorInfo.height = createInfo.renderPass->GetHeight();

        const VkPipelineShaderStageCreateInfo shaderStages[] { vertShaderModule.GetShaderStageCreateInfo(), fragShaderModule.GetShaderStageCreateInfo() };
        pipelineCreateInfo.shaderStages = shaderStages;
        pipelineCreateInfo.shaderStagesCount = 2;
        if (!s_QuadData.pipeline.Initialize(pipelineCreateInfo))
            return false;


        Vulkan::DescriptorPool* descriptorPool = device->GetDescriptorPool();
        s_QuadData.descriptorSet = descriptorPool->AllocateDescriptorSet(s_QuadData.bindingSetLayout);
        return true;
    }

    bool Renderer2D::BeginScene(Camera* camera)
    {
        m_Camera = camera;
        s_QuadData.vertices.Clear();
        s_QuadData.indices.Clear();
        s_QuadData.sprites.Clear();
        s_QuadData.uniforms.Clear();
        s_QuadData.quadCount = 0;

        return true;
    }

    void Renderer2D::EndScene()
    {
        Array<VkWriteDescriptorSet> writes;

        for (size_t i = 0; i < s_QuadData.quadCount; i++)
        {
            VkDescriptorBufferInfo bufferInfo;
            bufferInfo.buffer = s_QuadData.uniformBuffer.GetHandle();
            bufferInfo.offset = 0;
            bufferInfo.range = s_QuadData.quadCount * sizeof(QuadBatch::Uniform);

            VkWriteDescriptorSet uniform = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            uniform.dstSet = s_QuadData.descriptorSet;
            uniform.dstBinding = 0;
            uniform.dstArrayElement = i;
            uniform.descriptorCount = 1;
            uniform.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uniform.pBufferInfo = &bufferInfo;
            writes.Add(uniform);

            VkDescriptorImageInfo imageInfo;
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = ((Vulkan::Texture*)s_QuadData.sprites[s_QuadData.vertices[i * 4].spriteId].texture)->GetImageView();
            imageInfo.sampler = s_QuadData.sampler.GetHandle();


            VkWriteDescriptorSet image = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
            image.dstSet = s_QuadData.descriptorSet;
            image.dstBinding = 1;
            image.dstArrayElement = i;
            image.descriptorCount = 1;
            image.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            image.pImageInfo = &imageInfo;
            writes.Add(image);
        }

        vkUpdateDescriptorSets(((Vulkan::Device*)m_Device)->GetHandle(), writes.Count(), writes.Data(), 0, nullptr);

        s_QuadData.uboStaging.CPUCopy(s_QuadData.uniforms.Data(), 0, s_QuadData.uniforms.Size());
        s_QuadData.uboStaging.GPUCopy(s_QuadData.uniformBuffer, 0, 0, s_QuadData.uniforms.Size());

        s_QuadData.vertexBuffer.Destroy();
        s_QuadData.vertexBuffer = CreateVertexBuffer(*m_Device, s_QuadData.vertices.Data(), s_QuadData.vertices.Size());

        s_QuadData.indexBuffer.Destroy();
        s_QuadData.indexBuffer = CreateIndexBuffer(*m_Device, s_QuadData.indices.Data(), s_QuadData.indices.Size());
    }

    void Renderer2D::Render(Rendering::CommandBuffer& cmdBuffer)
    {
        cmdBuffer.BindGraphicsPipeline(s_QuadData.pipeline);
        cmdBuffer.BindVertexBuffer(s_QuadData.vertexBuffer, 0);
        cmdBuffer.BindIndexBuffer(s_QuadData.indexBuffer, 0, Format::Uint32);
        cmdBuffer.DrawIndexed(s_QuadData.indices.Count(), 0);
    }

    void Renderer2D::DrawSprite(const Matrix4& transform, const Sprite& sprite, const Vector2& tiling, const Color& colorTint, const SpriteRendererFlags& flags)
    {
        if (!sprite.texture)
            return;

        const float textureWidth = sprite.texture->GetWidth();
        const float textureHeight = sprite.texture->GetHeight();

        Vector2 uv0 = { sprite.x / (float)textureWidth, (sprite.y + sprite.height) / (float)textureHeight };
        Vector2 uv1 = { (sprite.x + sprite.width) / (float)textureWidth, (sprite.y + sprite.height) / (float)textureHeight };
        Vector2 uv2 = { (sprite.x + sprite.width) / (float)textureWidth, sprite.y / (float)textureHeight };
        Vector2 uv3 = { sprite.x / (float)textureWidth, sprite.y / (float)textureHeight };

        if(flags.Contains(SpriteRendererFlagBits::FlipHorizontal))
        {
            std::swap(uv0, uv1);
            std::swap(uv2, uv3);
        }

        if(flags.Contains(SpriteRendererFlagBits::FlipVertical))
        {
            std::swap(uv0, uv2);
            std::swap(uv1, uv3);
        }

        // TODO: Need to extract scale out of the World Space Matrix
        // TODO: Have pixels per unit parameter
        const Vector2 finalTiling = flags.Contains(SpriteRendererFlagBits::TileWithScale) ? tiling : tiling;
        const Vector2 spriteSize = Vector2(sprite.width, sprite.height);
        const Matrix2 spriteScale = sprite.texture ? Math::Scale(Matrix2::Identity, spriteSize / /* pixelsPerUnit */ 1.0f) : Matrix2::Identity;
        s_QuadData.uniforms.Add({ transform, finalTiling, Vector2::Zero, spriteScale, colorTint });
        s_QuadData.sprites.AddUnique(sprite);

        const QuadVertex vertices[4]
        {
            { Vector2(-0.5f, -0.5f), uv0, (uint32_t)s_QuadData.sprites.Find(sprite) },
            { Vector2(-0.5f, +0.5f), uv1, (uint32_t)s_QuadData.sprites.Find(sprite) },
            { Vector2(+0.5f, +0.5f), uv2, (uint32_t)s_QuadData.sprites.Find(sprite) },
            { Vector2(+0.5f, -0.5f), uv3, (uint32_t)s_QuadData.sprites.Find(sprite) },
        };

        const uint32_t indices[6]
        {
            s_QuadData.quadCount * 4 + 0,
            s_QuadData.quadCount * 4 + 2,
            s_QuadData.quadCount * 4 + 1,
            s_QuadData.quadCount * 4 + 0,
            s_QuadData.quadCount * 4 + 3,
            s_QuadData.quadCount * 4 + 2,
        };

        s_QuadData.vertices.AddRange(vertices, std::size(vertices));
        s_QuadData.indices.AddRange(indices, std::size(indices));
        s_QuadData.quadCount++;
    }
}
