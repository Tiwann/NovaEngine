#pragma once
#include "Runtime/LogCategory.h"
#include "Runtime/Filesystem.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(ShaderCompiler, "SHADER COMPILER")

namespace Nova
{
    enum class ShaderOptimization
    {
        None,
        Performance,
        Size
    };
    
    class ShaderCompiler
    {
    public:
        static void Compile(const Path& Filepath, Buffer<u8>& OutVertex, Buffer<u8>& OutIndex, ShaderOptimization Optimization);
    private:
        ShaderCompiler() = delete;
    };
}
