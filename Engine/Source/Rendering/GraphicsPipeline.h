#pragma once
#include "BlendFunction.h"
#include "ColorChannel.h"
#include "CompareOperation.h"
#include "CullMode.h"
#include "FrontFace.h"
#include "PolygonMode.h"
#include "Containers/Array.h"
#include "PrimitiveTopology.h"
#include "VertexLayout.h"

namespace Nova::Rendering
{
    class Device;

    struct GraphicsPipelineCreateInfo
    {
        struct InputAssemblyInfo
        {
            bool primitiveRestartEnable = false;
            PrimitiveTopology topology;
        } inputAssemblyInfo;

        struct VertexInputInfo
        {
            VertexLayout layout;
        } vertexInputInfo;

        struct RasterizationInfo
        {
            CullMode cullMode = CullMode::BackFace;
            FrontFace frontFace = FrontFace::Clockwise;
            PolygonMode polygonMode = PolygonMode::Fill;
            bool discardEnable = false;
            bool depthClampEnable = false;
            bool depthBiasEnable = false;;
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
            uint32_t x;
            uint32_t y;
            uint32_t width;
            uint32_t height;
            float minDepth;
            float maxDepth;
        } viewportInfo;

        struct ScissorInfo
        {
            uint32_t x;
            uint32_t y;
            uint32_t width;
            uint32_t height;
        } scissorInfo;

        Device* device;
        void* pipelineLayout = nullptr;
        const void* shaderStages = nullptr;
        size_t shaderStagesCount = 0;
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
