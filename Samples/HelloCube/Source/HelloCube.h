#pragma once
#include "Rendering/UniformBuffer.h"
#include "Runtime/Application.h"
#include "Runtime/EntityHandle.h"
#include "Runtime/LogCategory.h"

namespace Nova
{
    class Camera;
}

NOVA_DECLARE_LOG_CATEGORY_STATIC(HelloCube, "HELLO CUBE")

namespace Nova
{
    class Pipeline;
    class Shader;
    class VertexBuffer;
    class IndexBuffer;

    class HelloCube final : public Application
    {
    public:
        explicit HelloCube(const Array<const char*>& Arguments);
        ApplicationConfiguration CreateConfiguration() const override;
        void OnInit() override;
    };
}
