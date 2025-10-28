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

namespace Nova::Rendering
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

        GraphicsPipelineCreateInfo& setInputAssemblyInfo(const InputAssemblyInfo& inputAssembly) { inputAssemblyInfo = inputAssembly; return *this; }
        GraphicsPipelineCreateInfo& setVertexLayout(const VertexLayout& vertexLayout) { vertexInputInfo.layout = vertexLayout; return *this; }
        GraphicsPipelineCreateInfo& addVertexAttribute(const VertexAttribute& attribute) { vertexInputInfo.layout.AddAttribute(attribute); return *this; }
        GraphicsPipelineCreateInfo& setRasterizationInfo(const RasterizationInfo& rasterization) { rasterizationInfo = rasterization; return *this; }
        GraphicsPipelineCreateInfo& setColorBlendInfo(const ColorBlendInfo& colorBlend) { colorBlendInfo = colorBlend; return *this; }
        GraphicsPipelineCreateInfo& setDepthStencilInfo(const DepthStencilInfo& depthStencil) { depthStencilInfo = depthStencil; return *this; }
        GraphicsPipelineCreateInfo& setMultisampleInfo(const MultisampleInfo& multisample) { multisampleInfo = multisample; return *this; }
        GraphicsPipelineCreateInfo& setViewportInfo(const ViewportInfo& viewport) { viewportInfo = viewport; return *this; }
        GraphicsPipelineCreateInfo& setScissorInfo(const ScissorInfo& scissor) { scissorInfo = scissor; return *this; }
        GraphicsPipelineCreateInfo& setPrimitiveTopology(const PrimitiveTopology topology) { inputAssemblyInfo.topology = topology; return *this; }
        GraphicsPipelineCreateInfo& setCullMode(const CullMode cullMode) { rasterizationInfo.cullMode = cullMode; return *this; }
        GraphicsPipelineCreateInfo& setFrontFace(const FrontFace frontFace) { rasterizationInfo.frontFace = frontFace; return *this; }
        GraphicsPipelineCreateInfo& setPolygonMode(const PolygonMode polygonMode) { rasterizationInfo.polygonMode = polygonMode; return *this; }
        GraphicsPipelineCreateInfo& setShader(const Ref<Shader>& shader) { this->shader = shader; return *this; }
        GraphicsPipelineCreateInfo& setDevice(const Ref<Device>& device) { this->device = device; return *this; }
        GraphicsPipelineCreateInfo& setRenderPass(RenderPass* renderPass) { this->renderPass = renderPass; return *this; }
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
