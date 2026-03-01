#pragma once
#include "Common.h"
#include "Rendering/GraphicsPipeline.h"

namespace Nova::OpenGL
{
    class GraphicsPipeline final : public Nova::GraphicsPipeline
    {
    public:
        bool Initialize(const GraphicsPipelineCreateInfo& createInfo) override;
        void Destroy() override;
    protected:
        friend class Queue;
        void Bind() const;
    private:
        uint32_t m_VertexArrayObject = INVALID_HANDLE<uint32_t>;
        GraphicsPipelineCreateInfo m_PipelineDesc{};
    };
}
