#pragma once
#include "BlendFunction.h"
#include "ColorChannel.h"
#include "CompareOperation.h"
#include "CullMode.h"
#include "FrontFace.h"
#include "PolygonMode.h"
#include "PrimitiveTopology.h"
#include "VertexLayout.h"
#include "VertexInputBindingDesc.h"
#include "VertexInputAttributeDesc.h"

namespace Nova
{
    class RenderDevice;
    class RenderPass;
    class RenderTarget;
    class Shader;

    struct InputAssemblyState
    {
        bool primitiveRestartEnable = false;
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    };

    struct VertexInputState
    {
        Array<VertexInputBindingDesc> vertexInputBindings;
        Array<VertexInputAttributeDesc> vertexInputAttributes;
    };

    struct RasterizationState
    {
        CullMode cullMode = CullMode::BackFace;
        FrontFace frontFace = FrontFace::CounterClockwise;
        PolygonMode polygonMode = PolygonMode::Fill;
        bool discardEnable = false;
        bool depthClampEnable = false;
        bool depthBiasEnable = false;
        float depthBiasClamp = 0.0f;
        float depthBiasConstantFactor = 0.0f;
        float depthBiasSlopeFactor = 0.0f;
        float lineWidth = 1.0f;
    };

    struct ColorBlendState
    {
        bool colorBlendEnable = false;
        BlendFunction blendFunction;
        ColorChannelFlags colorWriteMask = ColorChannelFlags::All();
    };

    struct DepthStencilState
    {
        bool depthTestEnable = false;
        bool depthWriteEnable = false;
        bool stencilTestEnable = false;
        CompareOperation depthCompareOp = CompareOperation::Less;
    };

    struct MultisampleState
    {
        uint32_t sampleCount = 1;
        bool alphaToCoverageEnable = false;
        bool alphaToOneEnable = false;
        bool sampleShadingEnable = false;
    };

    struct ViewportState
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        float minDepth = 0.0f;
        float maxDepth = 1.0f;
    };

    struct ScissorState
    {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct GraphicsPipelineCreateInfo
    {
        InputAssemblyState inputAssemblyState;
        VertexInputState vertexInputState;
        RasterizationState rasterizationState;
        Array<ColorBlendState> colorBlendStates;
        DepthStencilState depthStencilState;
        MultisampleState multisampleState;
        ViewportState viewportState;
        ScissorState scissorState;
        Array<Format> colorAttachmentFormats;
        Format depthAttachmentFormat = Format::D32_FLOAT_S8_UINT;

        RenderDevice* device = nullptr;
        const Shader* shader = nullptr;

        GraphicsPipelineCreateInfo() = default;

        GraphicsPipelineCreateInfo& SetInputAssemblyInfo(const InputAssemblyState& inputAssembly) { inputAssemblyState = inputAssembly; return *this; }
        GraphicsPipelineCreateInfo& SetRasterizationInfo(const RasterizationState& rasterization) { rasterizationState = rasterization; return *this; }
        GraphicsPipelineCreateInfo& SetColorBlendInfo(const ColorBlendState& colorBlend) { this->colorBlendStates.Add(colorBlend); return *this; }
        GraphicsPipelineCreateInfo& SetDepthStencilInfo(const DepthStencilState& depthStencil) { depthStencilState = depthStencil; return *this; }
        GraphicsPipelineCreateInfo& SetMultisampleInfo(const MultisampleState& multisample) { multisampleState = multisample; return *this; }
        GraphicsPipelineCreateInfo& SetViewportInfo(const ViewportState& viewport) { viewportState = viewport; return *this; }
        GraphicsPipelineCreateInfo& SetScissorInfo(const ScissorState& scissor) { scissorState = scissor; return *this; }
        GraphicsPipelineCreateInfo& SetPrimitiveTopology(const PrimitiveTopology topology) { inputAssemblyState.topology = topology; return *this; }
        GraphicsPipelineCreateInfo& SetCullMode(const CullMode cullMode) { rasterizationState.cullMode = cullMode; return *this; }
        GraphicsPipelineCreateInfo& SetFrontFace(const FrontFace frontFace) { rasterizationState.frontFace = frontFace; return *this; }
        GraphicsPipelineCreateInfo& SetPolygonMode(const PolygonMode polygonMode) { rasterizationState.polygonMode = polygonMode; return *this; }
        GraphicsPipelineCreateInfo& SetShader(const Shader& inShader) { this->shader = &inShader; return *this; }
        GraphicsPipelineCreateInfo& SetDevice(RenderDevice* inDevice) { this->device = inDevice; return *this; }
    };

    VertexInputState CreateInputStateFromVertexLayout(const VertexLayout& vertexLayout);

    class GraphicsPipeline
    {
    public:
        GraphicsPipeline() = default;
        virtual ~GraphicsPipeline() = default;

        virtual bool Initialize(const GraphicsPipelineCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        RenderDevice* GetDevice() const { return m_Device; }
    private:
        RenderDevice* m_Device = nullptr;
    };
}
