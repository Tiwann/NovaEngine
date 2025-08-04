#pragma once
#include "Containers/Array.h"
#include "Containers/StringView.h"
#include "Rendering/ShaderStage.h"

#include <slang/slang.h>
#include <slang/slang-com-ptr.h>


namespace Nova
{
    enum class ShadingLanguage
    {
        Slang,
        GLSL,
        HLSL,
    };

    enum class ShaderTarget
    {
        SPIRV,
        GLSL,
        HLSL,
        DXBC,
        DXIL
    };

    struct EntryPoint
    {
        StringView name;
        ShaderStageFlagBits stage;
    };

    class ShaderCompiler
    {
    public:
        ShaderCompiler() = default;

        bool Initialize();
        void Destroy();

        void AddEntryPoint(const EntryPoint& entryPoint);
        void AddInclude(StringView include);
        void RemoveInclude(StringView include);

        bool Compile(StringView filepath, StringView moduleName, ShaderTarget target);

        Array<uint32_t> GetEntryPointBinary(StringView entryPointName) const;
        Array<uint32_t> GetEntryPointBinary(ShaderStageFlagBits stage) const;

    private:
        Array<StringView> m_IncludeDirs;
        Array<EntryPoint> m_EntryPoints;

        Slang::ComPtr<slang::ISession> m_Session = nullptr;
        Slang::ComPtr<slang::IModule> m_Module = nullptr;
        Array<Slang::ComPtr<slang::IEntryPoint>> m_EntryPointHandles;
        Slang::ComPtr<slang::IComponentType> m_Program = nullptr;
        Slang::ComPtr<slang::IComponentType> m_LinkedProgram = nullptr;

    };
}
