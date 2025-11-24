#pragma once
#include "BlendFunction.h"
#include "ColorChannel.h"
#include "CompareOperation.h"
#include "CullMode.h"
#include "FrontFace.h"
#include "PolygonMode.h"
#include "PrimitiveTopology.h"
#include "VertexLayout.h"
#include "Runtime/Ref.h"

namespace Nova
{
    class Device;
    class RenderPass;
    class RenderTarget;
    class Shader;

    struct GraphicsPipelineCreateInfo
    {
        struct InputAssemblyInfo
        {
            bool primitiveRestartEnable = false;
            PrimitiveTopology topology = PrimitiveTopology::TriangleList;
        } inputAssemblyInfo;

        struct VertexInputInfo
        {
            VertexLayout layout;
        } vertexInputInfo;

        struct RasterizationInfo
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
        } rasterizationInfo;

        struct ColorBlendInfo
        {
            bool colorBlendEnable = false;
            BlendFunction blendFunction;
            ColorChannelFlags colorWriteMask = ColorChannelFlags::All();
        } colorBlendInfo;

        struct DepthStencilInfo
        {
            bool depthTestEnable = false;
            bool depthWriteEnable = false;
            bool stencilTestEnable = false;
            CompareOperation depthCompareOp = CompareOperation::Less;
        } depthStencilInfo;

        struct MultisampleInfo
        {
            uint32_t sampleCount = 1;
            bool alphaToCoverageEnable = false;
            bool alphaToOneEnable = false;
            bool sampleShadingEnable = false;
        } multisampleInfo;

        struct ViewportInfo
        {
            uint32_t x = 0;
            uint32_t y = 0;
            uint32_t width = 0;
            uint32_t height = 0;
            float minDepth = 0.0f;
            float maxDepth = 1.0f;
        } viewportInfo;

        struct ScissorInfo
        {
            uint32_t x = 0;
            uint32_t y = 0;
            uint32_t width = 0;
            uint32_t height = 0;
        } scissorInfo;

        Device* device = nullptr;
        RenderPass* renderPass = nullptr;
        Shader* shader = nullptr;


        GraphicsPipelineCreateInfo() = default;

        GraphicsPipelineCreateInfo& SetInputAssemblyInfo(const InputAssemblyInfo& inputAssembly) { inputAssemblyInfo = inputAssembly; return *this; }
        GraphicsPipelineCreateInfo& SetVertexLayout(const VertexLayout& vertexLayout) { vertexInputInfo.layout = vertexLayout; return *this; }
        GraphicsPipelineCreateInfo& AddVertexAttribute(const VertexAttribute& attribute) { vertexInputInfo.layout.AddAttribute(attribute); return *this; }
        GraphicsPipelineCreateInfo& SetRasterizationInfo(const RasterizationInfo& rasterization) { rasterizationInfo = rasterization; return *this; }
        GraphicsPipelineCreateInfo& SetColorBlendInfo(const ColorBlendInfo& colorBlend) { colorBlendInfo = colorBlend; return *this; }
        GraphicsPipelineCreateInfo& SetDepthStencilInfo(const DepthStencilInfo& depthStencil) { depthStencilInfo = depthStencil; return *this; }
        GraphicsPipelineCreateInfo& SetMultisampleInfo(const MultisampleInfo& multisample) { multisampleInfo = multisample; return *this; }
        GraphicsPipelineCreateInfo& SetViewportInfo(const ViewportInfo& viewport) { viewportInfo = viewport; return *this; }
        GraphicsPipelineCreateInfo& SetScissorInfo(const ScissorInfo& scissor) { scissorInfo = scissor; return *this; }
        GraphicsPipelineCreateInfo& SetPrimitiveTopology(const PrimitiveTopology topology) { inputAssemblyInfo.topology = topology; return *this; }
        GraphicsPipelineCreateInfo& SetCullMode(const CullMode cullMode) { rasterizationInfo.cullMode = cullMode; return *this; }
        GraphicsPipelineCreateInfo& SetFrontFace(const FrontFace frontFace) { rasterizationInfo.frontFace = frontFace; return *this; }
        GraphicsPipelineCreateInfo& SetPolygonMode(const PolygonMode polygonMode) { rasterizationInfo.polygonMode = polygonMode; return *this; }
        GraphicsPipelineCreateInfo& SetShader(Shader* inShader) { this->shader = inShader; return *this; }
        GraphicsPipelineCreateInfo& SetDevice(Device* inDevice) { this->device = inDevice; return *this; }
        GraphicsPipelineCreateInfo& SetRenderPass(RenderPass* inRenderPass) { this->renderPass = inRenderPass; return *this; }
    };

    class GraphicsPipeline
    {
    public:
        GraphicsPipeline() = default;
        virtual ~GraphicsPipeline() = default;

        virtual bool Initialize(const GraphicsPipelineCreateInfo& createInfo) = 0;
        virtual void Destroy() = 0;
        Device* GetDevice() const { return m_Device; }
    private:
        Device* m_Device = nullptr;
    };
}
