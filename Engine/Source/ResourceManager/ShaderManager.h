#pragma once
#include "ResourceManager.h"
#include "Rendering/Shader.h"
#include "Runtime/LogCategory.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(ShaderManager, "SHADER MANAGER")

namespace Nova
{
    class ShaderManager : public ResourceManager<Shader>
    {
    public:
        ShaderManager() = default;

        Shader* Load(const String& Name, const Path& Filepath) override;
        Shader* Retrieve(const String& Name) override;
        void ReloadAll();
    };
}
