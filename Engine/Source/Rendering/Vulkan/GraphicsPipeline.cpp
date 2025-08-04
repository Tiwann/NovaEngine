#include "GraphicsPipeline.h"
#include "Device.h"
#include "Conversions.h"
#include "Rendering/RenderPass.h"
#include "Rendering/RenderTarget.h"

#include <vulkan/vulkan.h>

namespace Nova::Vulkan
{
    bool GraphicsPipeline::Initialize(const Rendering::GraphicsPipelineCreateInfo& createInfo)
    {
        m_Device = static_cast<Device*>(createInfo.device);
        VkDevice deviceHandle = m_Device->GetHandle();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyState.primitiveRestartEnable = createInfo.inputAssemblyInfo.primitiveRestartEnable;
        inputAssemblyState.topology = Convert<PrimitiveTopology, VkPrimitiveTopology>(createInfo.inputAssemblyInfo.topology);


        const Array<VertexAttribute>& vertexAttributes = createInfo.vertexInputInfo.layout.GetAttributes();
        Array<VkVertexInputAttributeDescription> attributeDescriptions;

        for (size_t i = 0; i < vertexAttributes.Count(); i++)
        {
            const VertexAttribute& vertexAttribute = vertexAttributes[i];
            VkVertexInputAttributeDescription attributeDesc;
            attributeDesc.binding = 0;
            attributeDesc.location = i;
            attributeDesc.format = Convert<Format, VkFormat>(vertexAttribute.format);
            attributeDesc.offset = createInfo.vertexInputInfo.layout.GetOffset(vertexAttribute);
            attributeDescriptions.Add(attributeDesc);
        }


        VkVertexInputBindingDescription bindingDescription;
        bindingDescription.binding = 0;
        bindingDescription.stride = createInfo.vertexInputInfo.layout.Stride();
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


        VkPipelineVertexInputStateCreateInfo vertexInputState { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.Data();
        vertexInputState.vertexAttributeDescriptionCount = attributeDescriptions.Count();
        vertexInputState.pVertexBindingDescriptions = &bindingDescription;
        vertexInputState.vertexBindingDescriptionCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizationState { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationState.cullMode = Convert<CullMode, VkCullModeFlags>(createInfo.rasterizationInfo.cullMode);
        rasterizationState.frontFace = Convert<FrontFace, VkFrontFace>(createInfo.rasterizationInfo.frontFace);
        rasterizationState.polygonMode = Convert<PolygonMode, VkPolygonMode>(createInfo.rasterizationInfo.polygonMode);
        rasterizationState.rasterizerDiscardEnable = createInfo.rasterizationInfo.discardEnable;
        rasterizationState.depthClampEnable = createInfo.rasterizationInfo.depthClampEnable;
        rasterizationState.depthBiasEnable = createInfo.rasterizationInfo.depthBiasEnable;
        rasterizationState.depthBiasClamp = createInfo.rasterizationInfo.depthBiasClamp;
        rasterizationState.depthBiasConstantFactor = createInfo.rasterizationInfo.depthBiasConstantFactor;
        rasterizationState.depthBiasSlopeFactor = createInfo.rasterizationInfo.depthBiasSlopeFactor;
        rasterizationState.lineWidth = createInfo.rasterizationInfo.lineWidth;

        VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
        colorBlendAttachmentState.blendEnable = createInfo.colorBlendInfo.colorBlendEnable;
        colorBlendAttachmentState.colorWriteMask = createInfo.colorBlendInfo.colorWriteMask;
        if (createInfo.colorBlendInfo.colorBlendEnable)
        {
            colorBlendAttachmentState.alphaBlendOp = Convert<BlendOperation, VkBlendOp>(createInfo.colorBlendInfo.blendFunction.alphaOp);
            colorBlendAttachmentState.colorBlendOp = Convert<BlendOperation, VkBlendOp>(createInfo.colorBlendInfo.blendFunction.colorOp);
            colorBlendAttachmentState.dstAlphaBlendFactor = Convert<BlendFactor, VkBlendFactor>(createInfo.colorBlendInfo.blendFunction.alphaDest);
            colorBlendAttachmentState.dstColorBlendFactor = Convert<BlendFactor, VkBlendFactor>(createInfo.colorBlendInfo.blendFunction.colorDest);
            colorBlendAttachmentState.srcAlphaBlendFactor = Convert<BlendFactor, VkBlendFactor>(createInfo.colorBlendInfo.blendFunction.alphaSource);
            colorBlendAttachmentState.srcColorBlendFactor = Convert<BlendFactor, VkBlendFactor>(createInfo.colorBlendInfo.blendFunction.colorSource);
        }


        VkPipelineColorBlendStateCreateInfo colorBlendState { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendState.pAttachments = &colorBlendAttachmentState;
        colorBlendState.attachmentCount = 1;


        VkPipelineDepthStencilStateCreateInfo depthStencilState { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencilState.depthTestEnable = createInfo.depthStencilInfo.depthTestEnable;
        depthStencilState.depthWriteEnable = createInfo.depthStencilInfo.depthWriteEnable;
        depthStencilState.stencilTestEnable = createInfo.depthStencilInfo.stencilTestEnable;
        depthStencilState.depthCompareOp = Convert<CompareOperation, VkCompareOp>(createInfo.depthStencilInfo.depthCompareOp);

        depthStencilState.depthBoundsTestEnable = false;
        depthStencilState.minDepthBounds = 0.0f; // Optional
        depthStencilState.maxDepthBounds = 1.0f; // Optional

        VkSampleMask SampleMask = 0xFFFFFFFF;
        VkPipelineMultisampleStateCreateInfo multisampleState { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleState.rasterizationSamples = (VkSampleCountFlagBits)createInfo.multisampleInfo.sampleCount;
        multisampleState.alphaToCoverageEnable = createInfo.multisampleInfo.alphaToCoverageEnable;
        multisampleState.alphaToOneEnable = createInfo.multisampleInfo.alphaToOneEnable;
        multisampleState.pSampleMask = &SampleMask;
        multisampleState.sampleShadingEnable = false;
        // MultisampleState.minSampleShading = ...; if sampleShading is true

        VkViewport viewport;
        viewport.x = createInfo.viewportInfo.x;
        viewport.y = createInfo.viewportInfo.y + createInfo.viewportInfo.height;
        viewport.width = createInfo.viewportInfo.width;
        viewport.height = -createInfo.viewportInfo.height;
        viewport.minDepth = createInfo.viewportInfo.minDepth;
        viewport.maxDepth = createInfo.viewportInfo.maxDepth;

        VkRect2D scissor;
        scissor.offset.x = createInfo.scissorInfo.x;
        scissor.offset.y = createInfo.scissorInfo.y;
        scissor.extent.width = createInfo.scissorInfo.width;
        scissor.extent.height = createInfo.scissorInfo.height;

        VkPipelineViewportStateCreateInfo viewportState { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        Array dynamicStates { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.dynamicStateCount = dynamicStates.Count();
        dynamicState.pDynamicStates =  dynamicStates.Data();

        Array<VkFormat> colorAttachmentFormats;
        for (size_t attachmentIndex = 0; attachmentIndex < createInfo.renderPass->GetAttachmentCount(); attachmentIndex++)
        {
            Rendering::RenderPassAttachment* attachment = createInfo.renderPass->GetAttachment(attachmentIndex);
            if (attachment->type != Rendering::AttachmentType::Color)
                continue;
            colorAttachmentFormats.Add(Convert<Format, VkFormat>(attachment->texture->GetFormat()));
        }


        VkPipelineRenderingCreateInfo renderingInfo { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
        renderingInfo.viewMask = 0;
        renderingInfo.colorAttachmentCount = createInfo.renderPass->GetColorAttachmentCount();
        renderingInfo.pColorAttachmentFormats = colorAttachmentFormats.Data();
        if (createInfo.renderPass->HasDepthAttachment())
        {
            const Rendering::RenderPassAttachment* depthAttachment = createInfo.renderPass->GetDepthAttachment();
            const Rendering::Texture* depthTexture = depthAttachment->texture;
            renderingInfo.depthAttachmentFormat = Convert<Format, VkFormat>(depthTexture->GetFormat());
            renderingInfo.stencilAttachmentFormat = Convert<Format, VkFormat>(depthTexture->GetFormat());
        }


        VkGraphicsPipelineCreateInfo pipelineCreateInfo { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipelineCreateInfo.pNext = &renderingInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineCreateInfo.pRasterizationState = &rasterizationState;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pColorBlendState = &colorBlendState;
        pipelineCreateInfo.pVertexInputState = &vertexInputState;
        pipelineCreateInfo.pDepthStencilState = &depthStencilState;
        pipelineCreateInfo.pDynamicState = &dynamicState;
        pipelineCreateInfo.pMultisampleState = &multisampleState;
        pipelineCreateInfo.renderPass = nullptr;
        pipelineCreateInfo.pStages = (const VkPipelineShaderStageCreateInfo*)createInfo.shaderStages;
        pipelineCreateInfo.stageCount = createInfo.shaderStagesCount;
        pipelineCreateInfo.layout = (VkPipelineLayout)createInfo.pipelineLayout;

        if (vkCreateGraphicsPipelines(deviceHandle, nullptr, 1, &pipelineCreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
            return false;
        return true;
    }

    void GraphicsPipeline::Destroy()
    {
        const VkDevice deviceHandle = m_Device->GetHandle();
        vkDestroyPipeline(deviceHandle, m_Handle, nullptr);
    }

    VkPipeline GraphicsPipeline::GetHandle() const
    {
        return m_Handle;
    }

    const VkPipeline* GraphicsPipeline::GetHandlePtr() const
    {
        return &m_Handle;
    }
}
