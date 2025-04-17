#include "Pipeline.h"

namespace Nova
{
    Pipeline::Pipeline(Renderer* Renderer, const PipelineSpecification& Specification)
        : m_Renderer(Renderer), m_Specification(Specification)
    {
    }
}
