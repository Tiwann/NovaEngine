#include "VulkanPipeline.h"
#include "VulkanRenderer.h"
#include "VulkanShader.h"
#include "Runtime/Log.h"
#include "VulkanRendererTypeConvertor.h"

namespace Nova
{
    VulkanPipeline::VulkanPipeline(Renderer* Renderer) : Pipeline(Renderer)
    {

    }

    bool VulkanPipeline::Initialize(const PipelineSpecification& Specification)
    {
        Pipeline::Initialize(Specification);

        VulkanRenderer* Renderer = m_Renderer->As<VulkanRenderer>();
        VkDevice Device = Renderer->GetDevice();
        const VulkanRendererTypeConvertor& Convertor = Renderer->Convertor;

        VkPipelineInputAssemblyStateCreateInfo InputAssemblyState { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
        InputAssemblyState.primitiveRestartEnable = Specification.PrimitiveRestartEnable;
        InputAssemblyState.topology = Convertor.ConvertPrimitiveTopology(Specification.PrimitiveTopology);

        const Array<VertexAttribute>& VertexAttributes = Specification.VertexLayout.GetAttributes();
        Array<VkVertexInputAttributeDescription> AttributeDescriptions;

        for (size_t i = 0; i < VertexAttributes.Count(); i++)
        {
            const VertexAttribute& VertexAttribute = VertexAttributes[i];
            VkVertexInputAttributeDescription AttributeDescription;
            AttributeDescription.binding = 0;
            AttributeDescription.location = i;
            AttributeDescription.format = Convertor.ConvertFormat(VertexAttribute.Format);
            AttributeDescription.offset = Specification.VertexLayout.GetOffset(VertexAttribute);
            AttributeDescriptions.Add(AttributeDescription);
        }


        VkVertexInputBindingDescription BindingDescription;
        BindingDescription.binding = 0;
        BindingDescription.stride = Specification.VertexLayout.Stride();
        BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


        VkPipelineVertexInputStateCreateInfo VertexInputState { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
        VertexInputState.pVertexAttributeDescriptions = AttributeDescriptions.Data();
        VertexInputState.vertexAttributeDescriptionCount = AttributeDescriptions.Count();
        VertexInputState.pVertexBindingDescriptions = &BindingDescription;
        VertexInputState.vertexBindingDescriptionCount = 1;

        VkPipelineRasterizationStateCreateInfo RasterizationState { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
        RasterizationState.cullMode = Convertor.ConvertCullMode(Specification.CullMode);
        RasterizationState.frontFace = Convertor.ConvertFrontFace(Specification.FrontFace);
        RasterizationState.polygonMode = Convertor.ConvertPolygonMode(Specification.PolygonMode);
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
            ColorBlendAttachmentState.alphaBlendOp = Convertor.ConvertBlendOperation(Specification.BlendFunction.AlphaOperation);
            ColorBlendAttachmentState.colorBlendOp = Convertor.ConvertBlendOperation(Specification.BlendFunction.ColorOperation);
            ColorBlendAttachmentState.dstAlphaBlendFactor = Convertor.ConvertBlendFactor(Specification.BlendFunction.AlphaDest);
            ColorBlendAttachmentState.dstColorBlendFactor = Convertor.ConvertBlendFactor(Specification.BlendFunction.ColorDest);
            ColorBlendAttachmentState.srcAlphaBlendFactor = Convertor.ConvertBlendFactor(Specification.BlendFunction.AlphaSource);
            ColorBlendAttachmentState.srcColorBlendFactor = Convertor.ConvertBlendFactor(Specification.BlendFunction.ColorSource);
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
        DepthStencilState.stencilTestEnable = Specification.StencilTestEnable;
        DepthStencilState.depthCompareOp = Convertor.ConvertCompareOperation(Specification.DepthCompareOperation);
        DepthStencilState.minDepthBounds = 0.0f;
        DepthStencilState.maxDepthBounds = 1.0f;

        VkSampleMask SampleMask = 0xFFFFFFFF;
        VkPipelineMultisampleStateCreateInfo MultisampleState { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
        MultisampleState.rasterizationSamples = (VkSampleCountFlagBits)Specification.RasterizationSamples;
        MultisampleState.alphaToCoverageEnable = Specification.AlphaToCoverageEnable;
        MultisampleState.alphaToOneEnable = Specification.AlphaToOneEnable;
        MultisampleState.pSampleMask = &SampleMask;
        MultisampleState.sampleShadingEnable = false;
        // MultisampleState.minSampleShading = ...; if sampleShading is true

        VkViewport Viewport;
        Viewport.x = Specification.Viewport.X;
        Viewport.y = Specification.Viewport.Y + Specification.Viewport.Height;
        Viewport.width = Specification.Viewport.Width;
        Viewport.height = -Specification.Viewport.Height;
        Viewport.minDepth = Specification.Viewport.MinDepth;
        Viewport.maxDepth = Specification.Viewport.MaxDepth;

        VkRect2D Scissor;
        Scissor.offset.x = Specification.Scissor.X;
        Scissor.offset.y = Specification.Scissor.Y;
        Scissor.extent.width = Specification.Scissor.Width;
        Scissor.extent.height = Specification.Scissor.Height;

        VkPipelineViewportStateCreateInfo ViewportState { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        ViewportState.viewportCount = 1;
        ViewportState.pViewports = &Viewport;
        ViewportState.scissorCount = 1;
        ViewportState.pScissors = &Scissor;

        Array<VkDynamicState> DynamicStates { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        VkPipelineDynamicStateCreateInfo DynamicState { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        DynamicState.dynamicStateCount = DynamicStates.Count();
        DynamicState.pDynamicStates =  DynamicStates.Data();


        Array<VkPipelineShaderStageCreateInfo> ShaderStages;
        if (Specification.ShaderProgram)
        {
            for (const VulkanShaderModule& Module : ((VulkanShader*)Specification.ShaderProgram)->GetShaderModules())
            {
                VkPipelineShaderStageCreateInfo ShaderStageCreateInfo { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
                ShaderStageCreateInfo.module = Module.Handle;
                ShaderStageCreateInfo.stage = Convertor.ConvertShaderStage(Module.Stage);
                ShaderStageCreateInfo.pName = "main";
                ShaderStageCreateInfo.pSpecializationInfo = nullptr;
                ShaderStages.Add(ShaderStageCreateInfo);
            }
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
        PipelineCreateInfo.layout = ((VulkanShader*)Specification.ShaderProgram)->GetPipelineLayout();

        if (VK_FAILED(vkCreateGraphicsPipelines(Device, nullptr, 1, &PipelineCreateInfo, nullptr, &m_Handle)))
        {
            NOVA_VULKAN_ERROR("Failed to create graphics pipeline!");
            return false;
        }

        return true;
    }

    void VulkanPipeline::Destroy()
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
