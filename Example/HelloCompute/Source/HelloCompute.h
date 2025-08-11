#pragma once
#include "Rendering/Vulkan/ComputePipeline.h"
#include "Runtime/Application.h"

using namespace Nova;
using namespace Nova::Rendering;
namespace Nova::Rendering { class Texture; class Shader;}

class HelloCompute : public Application
{
public:
    void OnInit() override;
    void OnPreRender(CommandBuffer& cmdBuffer) override;
private:
    Texture* m_Texture = nullptr;
    Shader* m_Shader = nullptr;
    Vulkan::ComputePipeline m_Pipeline;
};
