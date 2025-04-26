#include "Pipeline.h"

namespace Nova
{
    Pipeline::Pipeline(Renderer* Renderer)
        : Object("Graphics Pipeline"), m_Renderer(Renderer)
    {
    }

    bool Pipeline::Initialize(const PipelineSpecification& Specification)
    {
        m_Specification = Specification;
        return true;
    }

    const PipelineSpecification& Pipeline::GetSpecification() const
    {
        return m_Specification;
    }
}
