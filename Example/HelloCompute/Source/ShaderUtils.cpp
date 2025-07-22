#include "ShaderUtils.h"
#include <slang/slang.h>

namespace Nova
{
    void CompileShaderToSpirV(StringView filepath, Array<uint32_t>& vertSpirv, Array<uint32_t>& fragSpirv)
    {
        slang::IGlobalSession* slang = nullptr;
        slang::createGlobalSession(&slang);

        slang::TargetDesc shaderTargetDesc;
        shaderTargetDesc.structureSize = sizeof(slang::TargetDesc);
        shaderTargetDesc.format = SLANG_SPIRV;
        shaderTargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;
        shaderTargetDesc.lineDirectiveMode = SLANG_LINE_DIRECTIVE_MODE_DEFAULT;

        slang::SessionDesc sessionDesc;
        sessionDesc.targets = &shaderTargetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        slang::ISession* shaderCompiler = nullptr;
        SlangResult result = slang->createSession(sessionDesc, &shaderCompiler);
        if (SLANG_FAILED(result)) return;

        slang::IModule* shaderModule = shaderCompiler->loadModuleFromSource("HelloTriangle", *filepath, nullptr, nullptr);
        slang::IEntryPoint* vertEntryPoint = nullptr;
        slang::IEntryPoint* fragEntryPoint = nullptr;
        result = shaderModule->findAndCheckEntryPoint("vert", SLANG_STAGE_VERTEX, &vertEntryPoint, nullptr);
        if (SLANG_FAILED(result)) return;

        result = shaderModule->findAndCheckEntryPoint("frag", SLANG_STAGE_FRAGMENT, &fragEntryPoint, nullptr);
        if (SLANG_FAILED(result)) return;

        slang::IComponentType* entryPoints[] = { vertEntryPoint, fragEntryPoint };
        slang::IComponentType* shaderProgram = nullptr;
        result = shaderCompiler->createCompositeComponentType(entryPoints, 2, &shaderProgram, nullptr);
        if (SLANG_FAILED(result)) return;

        slang::IComponentType* linkedProgram = nullptr;
        result = shaderProgram->link(&linkedProgram, nullptr);
        if (SLANG_FAILED(result)) return;


        slang::IBlob* vertexData = nullptr, *fragmentData = nullptr;
        result = linkedProgram->getEntryPointCode(0, 0, &vertexData, nullptr);
        if (SLANG_FAILED(result)) return;

        result = linkedProgram->getEntryPointCode(1, 0, &fragmentData, nullptr);
        if (SLANG_FAILED(result)) return;

        vertSpirv = Array((const uint32_t*)vertexData->getBufferPointer(), vertexData->getBufferSize() / sizeof(uint32_t));
        fragSpirv = Array((const uint32_t*)fragmentData->getBufferPointer(), fragmentData->getBufferSize() / sizeof(uint32_t));
    }
}
