#include "VulkanPipeline.h"
#include "VulkanRenderer.h"
#include "VulkanShader.h"
#include "Runtime/Log.h"

namespace Nova
{
    VulkanPipeline::VulkanPipeline(Renderer* Renderer, const PipelineSpecification& Specification): Pipeline(Renderer, Specification)
    {
        VkDevice Device = ((VulkanRenderer*)Renderer)->GetDevice();

        VkPipelineInputAssemblyStateCreateInfo InputAssemblyState { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        InputAssemblyState.primitiveRestartEnable = Specification.PrimitiveRestartEnable;
        InputAssemblyState.topology = VulkanRenderer::ConvertTopology(Specification.PrimitiveTopology);

        const Array<VertexAttribute>& VertexAttributes = Specification.VertexBufferLayout.GetAttributes();
        Array<VkVertexInputAttributeDescription> AttributeDescriptions;

        for (size_t i = 0; i < VertexAttributes.Count(); i++)
        {
            const VertexAttribute& VertexAttribute = VertexAttributes[i];
            VkVertexInputAttributeDescription AttributeDescription;
            AttributeDescription.binding = 0;
            AttributeDescription.location = i;
            AttributeDescription.format = VulkanRenderer::ConvertFormat(VertexAttribute.Format);
            AttributeDescription.offset = Specification.VertexBufferLayout.GetOffset(VertexAttribute);
            AttributeDescriptions.Add(AttributeDescription);
        }


        VkVertexInputBindingDescription BindingDescription;
        BindingDescription.binding = 0;
        BindingDescription.stride = Specification.VertexBufferLayout.Stride();
        BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


        VkPipelineVertexInputStateCreateInfo VertexInputState { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        VertexInputState.pVertexAttributeDescriptions = AttributeDescriptions.Data();
        VertexInputState.vertexAttributeDescriptionCount = AttributeDescriptions.Count();
        VertexInputState.pVertexBindingDescriptions = &BindingDescription;
        VertexInputState.vertexBindingDescriptionCount = 1;

        VkPipelineRasterizationStateCreateInfo RasterizationState { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        RasterizationState.cullMode = VulkanRenderer::ConvertCullMode(Specification.CullMode);
        RasterizationState.frontFace = VulkanRenderer::ConvertFrontFace(Specification.FrontFace);
        RasterizationState.polygonMode = VulkanRenderer::ConvertPolygonMode(Specification.PolygonMode);
        RasterizationState.rasterizerDiscardEnable = Specification.RasterizerDiscardEnable;
        RasterizationState.depthClampEnable = Specification.DepthClampEnable;
        RasterizationState.depthBiasEnable = Specification.DepthBiasEnable;
        RasterizationState.depthBiasClamp = Specification.DepthBiasClamp;
        RasterizationState.depthBiasConstantFactor = Specification.DepthBiasConstantFactor;
        RasterizationState.depthBiasSlopeFactor = Specification.DepthBiasSlopeFactor;
        RasterizationState.lineWidth = Specification.LineWidth;

        VkPipelineColorBlendAttachmentState ColorBlendAttachmentState;
        ColorBlendAttachmentState.blendEnable = Specification.BlendEnable;
        if (Specification.BlendEnable)
        {
            ColorBlendAttachmentState.alphaBlendOp = VulkanRenderer::ConvertBlendOperation(Specification.BlendFunction.AlphaOperation);
            ColorBlendAttachmentState.colorBlendOp = VulkanRenderer::ConvertBlendOperation(Specification.BlendFunction.ColorOperation);
            ColorBlendAttachmentState.dstAlphaBlendFactor = VulkanRenderer::ConvertBlendFactor(Specification.BlendFunction.AlphaDest);
            ColorBlendAttachmentState.dstColorBlendFactor = VulkanRenderer::ConvertBlendFactor(Specification.BlendFunction.ColorDest);
            ColorBlendAttachmentState.srcAlphaBlendFactor = VulkanRenderer::ConvertBlendFactor(Specification.BlendFunction.AlphaSource);
            ColorBlendAttachmentState.srcColorBlendFactor = VulkanRenderer::ConvertBlendFactor(Specification.BlendFunction.ColorSource);
        }

        ColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;


        VkPipelineColorBlendStateCreateInfo ColorBlendState { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        ColorBlendState.pAttachments = &ColorBlendAttachmentState;
        ColorBlendState.attachmentCount = 1;
        ColorBlendState.logicOpEnable = false;
        // ColorBlendState.logicOp = ...;
        // ColorBlendState.blendConstants = ...;
        //ColorBlendState.blendConstants = ... ;

        VkPipelineDepthStencilStateCreateInfo DepthStencilState { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
        DepthStencilState.depthTestEnable = Specification.DepthTestEnable;
        DepthStencilState.depthWriteEnable = Specification.DepthWriteEnable;

        VkSampleMask SampleMask = 0xFFFFFFFF;
        VkPipelineMultisampleStateCreateInfo MultisampleState { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        MultisampleState.rasterizationSamples = (VkSampleCountFlagBits)Specification.RasterizationSamples;
        MultisampleState.alphaToCoverageEnable = false;
        MultisampleState.alphaToOneEnable = false;
        MultisampleState.pSampleMask = &SampleMask;
        MultisampleState.sampleShadingEnable = false;
        // MultisampleState.minSampleShading = ...; if sampleShading is true

        VkViewport Viewport;
        Viewport.x = Specification.Viewport.X;
        Viewport.y = Specification.Viewport.Y;
        Viewport.width = Specification.Viewport.Width;
        Viewport.height = Specification.Viewport.Height;
        Viewport.minDepth = Specification.Viewport.MinDepth;
        Viewport.maxDepth = Specification.Viewport.MaxDepth;

        VkRect2D Scissor;
        Scissor.offset.x = Specification.Scissor.Offset.X;
        Scissor.offset.y = Specification.Scissor.Offset.Y;
        Scissor.extent.width = Specification.Scissor.Extent.Width;
        Scissor.extent.height = Specification.Scissor.Extent.Height;

        VkPipelineViewportStateCreateInfo ViewportState { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        ViewportState.viewportCount = 1;
        ViewportState.pViewports = &Viewport;
        ViewportState.scissorCount = 1;
        ViewportState.pScissors = &Scissor;

        Array<VkDynamicState> DynamicStates {  };
        VkPipelineDynamicStateCreateInfo DynamicState { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        DynamicState.dynamicStateCount = DynamicStates.Count();
        DynamicState.pDynamicStates =  DynamicStates.Data();


        Array<VkPipelineShaderStageCreateInfo> ShaderStages;
        if (Specification.ShaderProgram)
        {
            for (const ShaderModule& Module : ((VulkanShader*)Specification.ShaderProgram)->GetShaderModules())
            {
                VkPipelineShaderStageCreateInfo ShaderStageCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
                ShaderStageCreateInfo.module = Module.Module;
                ShaderStageCreateInfo.stage = VulkanRenderer::ConvertShaderStage(Module.Stage);
                ShaderStageCreateInfo.pName = "main";
                ShaderStageCreateInfo.pSpecializationInfo = nullptr;
                ShaderStages.Add(ShaderStageCreateInfo);
            }
        }

        // Should get this from VulkanShader using Slang
        VkPipelineLayoutCreateInfo PipelineLayoutInfo { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
        PipelineLayoutInfo.pSetLayouts = nullptr;
        PipelineLayoutInfo.setLayoutCount = 0;
        PipelineLayoutInfo.pPushConstantRanges = nullptr;
        PipelineLayoutInfo.pushConstantRangeCount = 0;

        VkPipelineLayout PipelineLayout = nullptr;
        if (VK_FAILED(vkCreatePipelineLayout(Device, &PipelineLayoutInfo, nullptr, &PipelineLayout)))
        {
            NOVA_VULKAN_ERROR("Failed to create pipeline layout");
            return;
        }

        constexpr VkFormat Formats[] { VK_FORMAT_R8G8B8A8_UNORM };
        VkPipelineRenderingCreateInfo RenderingInfo { VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
        RenderingInfo.viewMask = 0;
        RenderingInfo.colorAttachmentCount = 1;
        RenderingInfo.pColorAttachmentFormats = Formats;
        RenderingInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
        RenderingInfo.stencilAttachmentFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;

        VkGraphicsPipelineCreateInfo PipelineCreateInfo { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        PipelineCreateInfo.pNext = &RenderingInfo;
        PipelineCreateInfo.pInputAssemblyState = &InputAssemblyState;
        PipelineCreateInfo.pRasterizationState = &RasterizationState;
        PipelineCreateInfo.pViewportState = &ViewportState;
        PipelineCreateInfo.pColorBlendState = &ColorBlendState;
        PipelineCreateInfo.pVertexInputState = &VertexInputState;
        PipelineCreateInfo.pDepthStencilState = &DepthStencilState;
        PipelineCreateInfo.pDynamicState = &DynamicState;
        PipelineCreateInfo.pMultisampleState = &MultisampleState;
        PipelineCreateInfo.renderPass = nullptr;
        PipelineCreateInfo.pStages = ShaderStages.Data();
        PipelineCreateInfo.stageCount = ShaderStages.Count();
        PipelineCreateInfo.layout = PipelineLayout;

        if (VK_FAILED(vkCreateGraphicsPipelines(Device, nullptr, 1, &PipelineCreateInfo, nullptr, &m_Handle)))
        {
            NOVA_VULKAN_ERROR("Failed to create graphics pipeline!");
            return;
        }
    }

    VulkanPipeline::~VulkanPipeline()
    {
        const VulkanRenderer* Renderer = dynamic_cast<VulkanRenderer*>(m_Renderer);
        const VkDevice Device = Renderer->GetDevice();
        vkDestroyPipeline(Device, m_Handle, nullptr);
    }

    VkPipeline VulkanPipeline::GetHandle() const
    {
        return m_Handle;
    }
}
