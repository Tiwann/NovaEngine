#include "ShaderUtils.h"
#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

namespace Nova
{
    static Slang::ComPtr<slang::IGlobalSession> slang = nullptr;

    void CompileShaderToSpirv(const StringView filepath, const StringView moduleName, Array<uint32_t>& vertSpirv, Array<uint32_t>& fragSpirv)
    {
        if (!slang) slang::createGlobalSession(slang.writeRef());
        if (!slang) return;


        slang::TargetDesc shaderTargetDesc;
        shaderTargetDesc.structureSize = sizeof(slang::TargetDesc);
        shaderTargetDesc.format = SLANG_SPIRV;
        shaderTargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;
        shaderTargetDesc.lineDirectiveMode = SLANG_LINE_DIRECTIVE_MODE_DEFAULT;

        slang::SessionDesc sessionDesc;
        sessionDesc.targets = &shaderTargetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

        Slang::ComPtr<slang::ISession> shaderCompiler = nullptr;
        SlangResult result = slang->createSession(sessionDesc, shaderCompiler.writeRef());
        if (SLANG_FAILED(result)) return;

        Slang::ComPtr<slang::IModule> shaderModule = nullptr;
        shaderModule = shaderCompiler->loadModuleFromSource(*moduleName, *filepath, nullptr, nullptr);

        Slang::ComPtr<slang::IEntryPoint> vertEntryPoint = nullptr;
        Slang::ComPtr<slang::IEntryPoint> fragEntryPoint = nullptr;
        result = shaderModule->findAndCheckEntryPoint("vert", SLANG_STAGE_VERTEX, vertEntryPoint.writeRef(), nullptr);
        if (SLANG_FAILED(result)) return;

        result = shaderModule->findAndCheckEntryPoint("frag", SLANG_STAGE_FRAGMENT, fragEntryPoint.writeRef(), nullptr);
        if (SLANG_FAILED(result)) return;

        slang::IComponentType* entryPoints[2] = { vertEntryPoint, fragEntryPoint };
        Slang::ComPtr<slang::IComponentType> shaderProgram = nullptr;
        result = shaderCompiler->createCompositeComponentType(entryPoints, std::size(entryPoints), shaderProgram.writeRef(), nullptr);
        if (SLANG_FAILED(result)) return;

        Slang::ComPtr<slang::IComponentType> linkedProgram = nullptr;
        result = shaderProgram->link(linkedProgram.writeRef(), nullptr);
        if (SLANG_FAILED(result)) return;

       Slang::ComPtr<slang::IBlob> vertexData = nullptr, fragmentData = nullptr;
        result = linkedProgram->getEntryPointCode(0, 0, vertexData.writeRef(), nullptr);
        if (SLANG_FAILED(result)) return;

        result = linkedProgram->getEntryPointCode(1, 0, fragmentData.writeRef(), nullptr);
        if (SLANG_FAILED(result)) return;

        vertSpirv = Array((const uint32_t*)vertexData->getBufferPointer(), vertexData->getBufferSize() / sizeof(uint32_t));
        fragSpirv = Array((const uint32_t*)fragmentData->getBufferPointer(), fragmentData->getBufferSize() / sizeof(uint32_t));
    }

    void CompileShaderToSpirv(const StringView filepath, const StringView moduleName, Array<uint32_t>& computeSpirv)
    {
        if (!slang) createGlobalSession(slang.writeRef());

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
