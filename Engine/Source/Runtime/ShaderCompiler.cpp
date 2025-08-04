#include "ShaderCompiler.h"
#include "Containers/StringFormat.h"
#include <print>

namespace Nova
{
    static Slang::ComPtr<slang::IGlobalSession> slang = nullptr;

    static SlangCompileTarget GetCompileTarget(ShaderTarget target)
    {
        switch (target)
        {
        case ShaderTarget::SPIRV: return SLANG_SPIRV;
        case ShaderTarget::GLSL: return SLANG_GLSL;
        case ShaderTarget::HLSL: return SLANG_HLSL;
        case ShaderTarget::DXBC: return SLANG_DXBC;
        case ShaderTarget::DXIL: return SLANG_DXIL;
        default: return SLANG_SPIRV;
        }
    }

    static SlangStage GetStage(const ShaderStageFlagBits stage)
    {
        switch (stage)
        {
        case ShaderStageFlagBits::None: return SLANG_STAGE_NONE;
        case ShaderStageFlagBits::Vertex: return SLANG_STAGE_VERTEX;
        case ShaderStageFlagBits::Geometry: return SLANG_STAGE_GEOMETRY;
        case ShaderStageFlagBits::Fragment: return SLANG_STAGE_FRAGMENT;
        case ShaderStageFlagBits::Compute: return SLANG_STAGE_COMPUTE;
        case ShaderStageFlagBits::RayGeneration: return SLANG_STAGE_RAY_GENERATION;
        case ShaderStageFlagBits::Tessellation: return SLANG_STAGE_HULL;
        case ShaderStageFlagBits::Mesh: return SLANG_STAGE_MESH;
        default: return SLANG_STAGE_NONE;
        }
    }

    StringView GetErrorString(const Slang::ComPtr<slang::IBlob>& blob)
    {
        const StringView errorString = { (const char*)blob->getBufferPointer(), blob->getBufferSize() };
        return errorString;
    };

    bool ShaderCompiler::Initialize()
    {
        if (!slang)
        {
            SlangResult result = slang::createGlobalSession(slang.writeRef());
            if (SLANG_FAILED(result))
                return false;
        }
        return true;
    }

    void ShaderCompiler::Destroy()
    {
        m_LinkedProgram->release();
        m_Program->release();
        for (const auto& entryPoint : m_EntryPointHandles)
            entryPoint->release();
        m_Module->release();
        m_Session->release();
    }

    void ShaderCompiler::AddEntryPoint(const EntryPoint& entryPoint)
    {
        m_EntryPoints.Add(entryPoint);
    }


    void ShaderCompiler::AddInclude(const StringView include)
    {
        m_IncludeDirs.Add(include);
    }

    void ShaderCompiler::RemoveInclude(StringView include)
    {
        m_IncludeDirs.Remove(include);
    }

    bool ShaderCompiler::Compile(const StringView filepath, const StringView moduleName, const ShaderTarget target)
    {
        slang::TargetDesc shaderTargetDesc;
        shaderTargetDesc.format = GetCompileTarget(target);
        shaderTargetDesc.floatingPointMode = SLANG_FLOATING_POINT_MODE_DEFAULT;
        shaderTargetDesc.lineDirectiveMode = SLANG_LINE_DIRECTIVE_MODE_DEFAULT;

        slang::SessionDesc sessionDesc;
        sessionDesc.targets = &shaderTargetDesc;
        sessionDesc.targetCount = 1;
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
        sessionDesc.searchPaths = m_IncludeDirs.Transform<const char*>([](const StringView includeDir) { return *includeDir; }).Data();
        sessionDesc.searchPathCount = m_IncludeDirs.Count();

        SlangResult result = slang->createSession(sessionDesc, m_Session.writeRef());
        if (SLANG_FAILED(result)) return false;

        Slang::ComPtr<slang::IBlob> errorBlob = nullptr;


        m_Module = m_Session->loadModuleFromSource(*moduleName, *filepath, nullptr, errorBlob.writeRef());
        if (!m_Module)
        {
            std::println(std::cerr, "Failed to load slang module: {}", GetErrorString(errorBlob));
            return false;
        }

        for (const EntryPoint& entryPoint : m_EntryPoints)
        {
            Slang::ComPtr<slang::IEntryPoint> handle = nullptr;
            result = m_Module->findAndCheckEntryPoint(*entryPoint.name, GetStage(entryPoint.stage), handle.writeRef(), errorBlob.writeRef());
            if (SLANG_FAILED(result))
                continue;

            m_EntryPointHandles.Add(handle);
        }

        Array<slang::IComponentType*> entryPoints;
        for (const auto& entryPoint : m_EntryPointHandles)
            entryPoints.Add(entryPoint.get());

        result = m_Session->createCompositeComponentType(entryPoints.Data(), entryPoints.Count(), m_Program.writeRef(), errorBlob.writeRef());
        if (SLANG_FAILED(result))
        {
            std::println(std::cerr, "Failed to create shader program: {}", GetErrorString(errorBlob));
            return false;
        }

        result = m_Program->link(m_LinkedProgram.writeRef(), errorBlob.writeRef());
        if (SLANG_FAILED(result))
        {
            std::println(std::cerr, "Failed to link shader program: {}", GetErrorString(errorBlob));
            return false;
        }
        return true;
    }

    Array<uint32_t> ShaderCompiler::GetEntryPointBinary(const StringView entryPointName) const
    {
        const Array<StringView> names = m_EntryPoints.Transform<StringView>([](const EntryPoint& entryPoint) { return entryPoint.name; });
        const size_t index = names.Find(entryPointName);
        if (index == ~0)
            return {};

        Slang::ComPtr<slang::IBlob> errorBlob = nullptr;
        Slang::ComPtr<slang::IBlob> entryPointCode = nullptr;
        const SlangResult result = m_LinkedProgram->getEntryPointCode(index, 0, entryPointCode.writeRef(), errorBlob.writeRef());
        if (SLANG_FAILED(result))
        {
            std::println(std::cerr, "Failed to get entry point code: {}", GetErrorString(errorBlob));
            return {};
        }

        return Array((uint32_t*)entryPointCode->getBufferPointer(), entryPointCode->getBufferSize() / sizeof(uint32_t));
    }

    Array<uint32_t> ShaderCompiler::GetEntryPointBinary(const ShaderStageFlagBits stage) const
    {
        const Array<ShaderStageFlagBits> stages = m_EntryPoints.Transform<ShaderStageFlagBits>([](const EntryPoint& entryPoint) { return entryPoint.stage; });
        const size_t index = stages.Find(stage);
        if (index == ~0)
            return {};

        Slang::ComPtr<slang::IBlob> errorBlob = nullptr;
        Slang::ComPtr<slang::IBlob> entryPointCode = nullptr;
        const SlangResult result = m_LinkedProgram->getEntryPointCode(index, 0, entryPointCode.writeRef(), errorBlob.writeRef());
        if (SLANG_FAILED(result))
        {
            std::println(std::cerr, "Failed to get entry point code: {}", GetErrorString(errorBlob));
            return {};
        }

        return Array((uint32_t*)entryPointCode->getBufferPointer(), entryPointCode->getBufferSize() / sizeof(uint32_t));
    }
}
