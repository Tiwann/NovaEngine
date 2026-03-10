#include "GraphicsPipeline.h"
#include "RenderDevice.h"
#include "Conversions.h"
#include "Runtime/Log.h"
#include "Shader.h"
#include "ShaderModule.h"

#include <vulkan/vulkan.h>

#define CHECK_VALUE(value, message, ...) \
        do { \
            if (!(value)) \
            { \
                NOVA_LOG(RenderDevice, Verbosity::Error, message, __VA_ARGS__); \
                return false; \
            } \
        } while(0)

namespace Nova::Vulkan
{
    bool GraphicsPipeline::Initialize(const GraphicsPipelineCreateInfo& createInfo)
    {
        CHECK_VALUE(createInfo.device, "Failed to create Graphics Pipeline: invalid render device!");
        CHECK_VALUE(createInfo.shader, "Failed to create Graphics Pipeline: invalid shader!");

        RenderDevice* device = static_cast<RenderDevice*>(createInfo.device);
        VkDevice deviceHandle = device->GetHandle();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        inputAssemblyState.primitiveRestartEnable = createInfo.inputAssemblyState.primitiveRestartEnable;
        inputAssemblyState.topology = Convert<VkPrimitiveTopology>(createInfo.inputAssemblyState.topology);

        const auto ToVkBindingDesc = [](const VertexInputBindingDesc& binding)
        {
            VkVertexInputBindingDescription desc;
            desc.binding = binding.binding;
            desc.stride = binding.stride;
            desc.inputRate = Convert<VkVertexInputRate>(binding.inputRate);
            return desc;
        };

        const auto ToVkAttributeDesc = [](const VertexInputAttributeDesc& attribue)
        {
            VkVertexInputAttributeDescription desc;
            desc.binding = attribue.binding;
            desc.location = attribue.location;
            desc.format = Convert<VkFormat>(attribue.format);
            desc.offset = attribue.offset;
            return desc;
        };

        Array<VkVertexInputBindingDescription> bindingDescriptions = createInfo.vertexInputState.vertexInputBindings
        .Transform<VkVertexInputBindingDescription>(ToVkBindingDesc);

        Array<VkVertexInputAttributeDescription> attributeDescriptions = createInfo.vertexInputState.vertexInputAttributes
        .Transform<VkVertexInputAttributeDescription>(ToVkAttributeDesc);

        VkPipelineVertexInputStateCreateInfo vertexInputState { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.Data();
        vertexInputState.vertexAttributeDescriptionCount = attributeDescriptions.Count();
        vertexInputState.pVertexBindingDescriptions = bindingDescriptions.Data();
        vertexInputState.vertexBindingDescriptionCount = bindingDescriptions.Count();

        VkPipelineRasterizationStateCreateInfo rasterizationState { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        rasterizationState.cullMode = Convert<VkCullModeFlags>(createInfo.rasterizationState.cullMode);
        rasterizationState.frontFace = Convert<VkFrontFace>(createInfo.rasterizationState.frontFace);
        rasterizationState.polygonMode = Convert<VkPolygonMode>(createInfo.rasterizationState.polygonMode);
        rasterizationState.rasterizerDiscardEnable = createInfo.rasterizationState.discardEnable;
        rasterizationState.depthClampEnable = createInfo.rasterizationState.depthClampEnable;
        rasterizationState.depthBiasEnable = createInfo.rasterizationState.depthBiasEnable;
        rasterizationState.depthBiasClamp = createInfo.rasterizationState.depthBiasClamp;
        rasterizationState.depthBiasConstantFactor = createInfo.rasterizationState.depthBiasConstantFactor;
        rasterizationState.depthBiasSlopeFactor = createInfo.rasterizationState.depthBiasSlopeFactor;
        rasterizationState.lineWidth = createInfo.rasterizationState.lineWidth;

        Array<VkPipelineColorBlendAttachmentState> colorBlendStates;
        for (const auto& [colorBlendEnable, blendFunction, colorWriteMask] : createInfo.colorBlendStates)
        {
            VkPipelineColorBlendAttachmentState state;
            state.blendEnable = colorBlendEnable;
            state.colorWriteMask = colorWriteMask;
            if (colorBlendEnable)
            {
                state.alphaBlendOp = Convert<VkBlendOp>(blendFunction.alphaOp);
                state.colorBlendOp = Convert<VkBlendOp>(blendFunction.colorOp);
                state.dstAlphaBlendFactor = Convert<VkBlendFactor>(blendFunction.alphaDest);
                state.dstColorBlendFactor = Convert<VkBlendFactor>(blendFunction.colorDest);
                state.srcAlphaBlendFactor = Convert<VkBlendFactor>(blendFunction.alphaSource);
                state.srcColorBlendFactor = Convert<VkBlendFactor>(blendFunction.colorSource);
            }

            colorBlendStates.Add(state);
        }



        VkPipelineColorBlendStateCreateInfo colorBlendState { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        colorBlendState.pAttachments = colorBlendStates.Data();
        colorBlendState.attachmentCount = colorBlendStates.Count();

        VkPipelineDepthStencilStateCreateInfo depthStencilState { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        depthStencilState.depthTestEnable = createInfo.depthStencilState.depthTestEnable;
        depthStencilState.depthWriteEnable = createInfo.depthStencilState.depthWriteEnable;
        depthStencilState.stencilTestEnable = createInfo.depthStencilState.stencilTestEnable;
        depthStencilState.depthCompareOp = Convert<VkCompareOp>(createInfo.depthStencilState.depthCompareOp);

        depthStencilState.depthBoundsTestEnable = false;
        depthStencilState.minDepthBounds = 0.0f; // Optional
        depthStencilState.maxDepthBounds = 1.0f; // Optional

        VkSampleMask SampleMask = 0xFFFFFFFF;
        VkPipelineMultisampleStateCreateInfo multisampleState { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        multisampleState.rasterizationSamples = (VkSampleCountFlagBits)createInfo.multisampleState.sampleCount;
        multisampleState.alphaToCoverageEnable = createInfo.multisampleState.alphaToCoverageEnable;
        multisampleState.alphaToOneEnable = createInfo.multisampleState.alphaToOneEnable;
        multisampleState.pSampleMask = &SampleMask;
        multisampleState.sampleShadingEnable = false;
        // MultisampleState.minSampleShading = ...; if sampleShading is true

        VkViewport viewport;
        viewport.x = createInfo.viewportState.x;
        viewport.y = createInfo.viewportState.y + createInfo.viewportState.height;
        viewport.width = createInfo.viewportState.width;
        viewport.height = -createInfo.viewportState.height;
        viewport.minDepth = createInfo.viewportState.minDepth;
        viewport.maxDepth = createInfo.viewportState.maxDepth;

        VkRect2D scissor;
        scissor.offset.x = createInfo.scissorState.x;
        scissor.offset.y = createInfo.scissorState.y;
        scissor.extent.width = createInfo.scissorState.width;
        scissor.extent.height = createInfo.scissorState.height;

        VkPipelineViewportStateCreateInfo viewportState { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        Array dynamicStates { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo dynamicState { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicState.dynamicStateCount = dynamicStates.Count();
        dynamicState.pDynamicStates =  dynamicStates.Data();

        Array<VkFormat> colorAttachmentFormats = createInfo.colorAttachmentFormats.Transform<VkFormat>([](const Format& format) { return Convert<VkFormat>(format); });
        VkFormat depthAttachmentFormat = Convert<VkFormat>(createInfo.depthAttachmentFormat);

        VkPipelineRenderingCreateInfo renderingInfo { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
        renderingInfo.viewMask = 0;
        renderingInfo.colorAttachmentCount = colorAttachmentFormats.Count();
        renderingInfo.pColorAttachmentFormats = colorAttachmentFormats.Data();
        renderingInfo.depthAttachmentFormat = depthAttachmentFormat;
        renderingInfo.stencilAttachmentFormat = depthAttachmentFormat;

        Array<VkPipelineShaderStageCreateInfo> shaderStages;
        for (const ShaderModule& shaderModule : ((Shader*)createInfo.shader)->GetShaderModules())
        {
            VkPipelineShaderStageCreateInfo shaderStage { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            shaderStage.module = shaderModule.GetHandle();
            shaderStage.pName = "main";
            shaderStage.stage = Convert<VkShaderStageFlagBits>(shaderModule.GetStage());
            shaderStages.Add(shaderStage);
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
        pipelineCreateInfo.pStages = shaderStages.Data();
        pipelineCreateInfo.stageCount = shaderStages.Count();
        pipelineCreateInfo.layout = ((Shader*)createInfo.shader)->GetPipelineLayout();

        if (m_Handle)
            vkDestroyPipeline(deviceHandle, m_Handle, nullptr);

        if (vkCreateGraphicsPipelines(deviceHandle, nullptr, 1, &pipelineCreateInfo, nullptr, &m_Handle) != VK_SUCCESS)
            return false;

        m_Device = device;
        return true;
    }

    void GraphicsPipeline::Destroy()
    {
        NOVA_ASSERT(m_Device, "Device is null. Object may not be initialized!");
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

#undef CHECK_VALUE