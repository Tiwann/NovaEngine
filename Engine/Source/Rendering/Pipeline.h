#pragma once
#include "BlendFunction.h"
#include "CullMode.h"
#include "FrontFace.h"
#include "PolygonMode.h"
#include "PrimitiveTopology.h"
#include "Scissor.h"
#include "VertexBufferLayout.h"
#include "Viewport.h"
#include "Runtime/Object.h"


namespace Nova
{
    class Shader;

    struct PipelineSpecification
    {
        VertexBufferLayout VertexBufferLayout;
        bool RasterizerDiscardEnable = true;
        bool PrimitiveRestartEnable = false;
        CullMode CullMode = CullMode::None;
        FrontFace FrontFace = FrontFace::Clockwise;
        PolygonMode PolygonMode = PolygonMode::Fill;
        PrimitiveTopology PrimitiveTopology = PrimitiveTopology::TriangleList;
        bool DepthTestEnable = false;
        bool DepthWriteEnable = false;
        bool StencilTestEnable = false;
        bool DepthBiasEnable = false;
        float DepthBiasClamp = false;
        bool DepthClampEnable = false;
        float DepthBiasConstantFactor = 0.0f;
        float DepthBiasSlopeFactor = 0.0f;
        float LineWidth = 1.0f;
        Viewport Viewport;
        Scissor Scissor;
        u8 RasterizationSamples;
        bool BlendEnable = false;
        BlendFunction BlendFunction;
        bool DynamicRendering = false;
        Shader* ShaderProgram = nullptr;
    };

    class Renderer;

    class Pipeline : public Object
    {
    protected:
        Pipeline(Renderer* Renderer, const PipelineSpecification& Specification);

        Renderer* m_Renderer = nullptr;
        PipelineSpecification m_Specification;
    };
}