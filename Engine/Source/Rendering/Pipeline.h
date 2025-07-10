#pragma once
#include "BlendFunction.h"
#include "CompareOperation.h"
#include "CullMode.h"
#include "FrontFace.h"
#include "Multisample.h"
#include "PolygonMode.h"
#include "PrimitiveTopology.h"
#include "Scissor.h"
#include "VertexLayout.h"
#include "Viewport.h"
#include "Runtime/Object.h"


namespace Nova
{
    class Shader;

    struct PipelineCreateInfo
    {
        VertexLayout VertexLayout;
        bool RasterizerDiscardEnable = true;
        bool PrimitiveRestartEnable = false;
        CullMode CullMode = CullMode::None;
        FrontFace FrontFace = FrontFace::Clockwise;
        PolygonMode PolygonMode = PolygonMode::Fill;
        PrimitiveTopology PrimitiveTopology = PrimitiveTopology::TriangleList;
        bool DepthTestEnable = false;
        bool DepthWriteEnable = false;
        CompareOperation DepthCompareOperation = CompareOperation::Less;
        bool StencilTestEnable = false;
        bool DepthBiasEnable = false;
        bool AlphaToCoverageEnable = false;
        bool AlphaToOneEnable = false;
        float DepthBiasClamp = false;
        bool DepthClampEnable = false;
        float DepthBiasConstantFactor = 0.0f;
        float DepthBiasSlopeFactor = 0.0f;
        float LineWidth = 1.0f;
        Viewport Viewport;
        Scissor Scissor;
        SampleCount RasterizationSamples = SampleCount::S1;
        bool BlendEnable = false;
        BlendFunction BlendFunction;
        bool DynamicRendering = false;
        Shader* ShaderProgram = nullptr;
        bool MultisampleEnable = false;
        void* VulkanPipelineLayout = nullptr;
    };

    class Renderer;

    class Pipeline : public Object
    {
    public:
        virtual bool Initialize(const PipelineCreateInfo& Specification);
        virtual void Destroy() = 0;

        const PipelineCreateInfo& GetSpecification() const;
    protected:
        explicit Pipeline(Renderer* Renderer);

        Renderer* m_Renderer = nullptr;
        PipelineCreateInfo m_Specification;
    };
}