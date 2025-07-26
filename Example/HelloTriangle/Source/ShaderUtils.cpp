#include "ShaderUtils.h"
#include <slang/slang.h>

namespace Nova
{
    static slang::IGlobalSession* slang = nullptr;

    void CompileShaderToSpirv(StringView filepath, StringView moduleName, Array<uint32_t>& vertSpirv, Array<uint32_t>& fragSpirv)
    {
        if (!slang) slang::createGlobalSession(&slang);

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

        slang::IModule* shaderModule = shaderCompiler->loadModuleFromSource(*moduleName, *filepath, nullptr, nullptr);
        slang::IEntryPoint* vertEntryPoint = nullptr;
        slang::IEntryPoint* fragEntryPoint = nullptr;
        result = shaderModule->findAndCheckEntryPoint("vert", SLANG_STAGE_VERTEX, &vertEntryPoint, nullptr);
        if (SLANG_FAILED(result)) return;

        result = shaderModule->findAndCheckEntryPoint("frag", SLANG_STAGE_FRAGMENT, &fragEntryPoint, nullptr);
        if (SLANG_FAILED(result)) return;

        slang::IComponentType* entryPoints[2] = { vertEntryPoint, fragEntryPoint };
        slang::IComponentType* shaderProgram = nullptr;
        result = shaderCompiler->createCompositeComponentType(entryPoints, std::size(entryPoints), &shaderProgram, nullptr);
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

        linkedProgram->release();
        shaderProgram->release();
        shaderModule->release();
        shaderCompiler->release();
    }

    void CompileShaderToSpirv(StringView filepath, StringView moduleName, Array<uint32_t>& computeSpirv)
    {
        if (!slang) slang::createGlobalSession(&slang);

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

        slang::IModule* shaderModule = shaderCompiler->loadModuleFromSource(*moduleName, *filepath, nullptr, nullptr);
        slang::IEntryPoint* computeEntryPoint = nullptr;
        result = shaderModule->findAndCheckEntryPoint("compute", SLANG_STAGE_COMPUTE, &computeEntryPoint, nullptr);
        if (SLANG_FAILED(result)) return;

        slang::IComponentType* entryPoints[1] = { computeEntryPoint };
        slang::IComponentType* shaderProgram = nullptr;
        result = shaderCompiler->createCompositeComponentType(entryPoints, std::size(entryPoints), &shaderProgram, nullptr);
        if (SLANG_FAILED(result)) return;

        slang::IComponentType* linkedProgram = nullptr;
        result = shaderProgram->link(&linkedProgram, nullptr);
        if (SLANG_FAILED(result)) return;


        slang::IBlob* spirv = nullptr;
        result = linkedProgram->getEntryPointCode(0, 0, &spirv, nullptr);
        if (SLANG_FAILED(result)) return;

        computeSpirv = Array((const uint32_t*)spirv->getBufferPointer(), spirv->getBufferSize());

        linkedProgram->release();
        shaderProgram->release();
        shaderModule->release();
        shaderCompiler->release();
    }
}
