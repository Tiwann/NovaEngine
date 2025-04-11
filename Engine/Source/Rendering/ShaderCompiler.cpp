#include "ShaderCompiler.h"
#include "Runtime/Application.h"
#include "Rendering/Renderer.h"
#include <slang/slang.h>

namespace Nova
{
    void ShaderCompiler::Compile(const Path& Filepath, Buffer<u8>& OutVertex, Buffer<u8>& OutFragment, ShaderOptimization Optimization)
{
    SlangSession* Session = g_Application->GetSlangSession();

    const ScopedBuffer ShaderContent = File::ReadToBuffer(Filepath);
    const BufferView<char> ContentView = BufferView(ShaderContent.AsBuffer()).As<char>();

    SlangCompileRequest* Request = spCreateCompileRequest(Session);
    spSetCodeGenTarget(Request, SLANG_SPIRV);
    spSetCompileFlags(Request, 0);

    const int Index = spAddTranslationUnit(Request, SLANG_SOURCE_LANGUAGE_SLANG, nullptr);
    spAddTranslationUnitSourceStringSpan(Request, Index, Filepath.string().c_str(), ContentView.As<char>().Data(), ContentView.Data() + ContentView.Count());
    const int VertEntry = spAddEntryPoint(Request, Index, "vert", SLANG_STAGE_VERTEX);
    const int FragEntry = spAddEntryPoint(Request, Index, "frag", SLANG_STAGE_FRAGMENT);

    const SlangResult Result = spCompile(Request);
    if (SLANG_FAILED(Result))
    {
        // Get error log
        const char* diagnostics = spGetDiagnosticOutput(Request);
        std::cerr << "Shader Compilation Failed:\n" << diagnostics << std::endl;
    }
    else
    {
        // Get compiled code
        size_t codeSize = 0;
        const void* compiledCodeVert = spGetEntryPointCode(Request, VertEntry, &codeSize);
        if (compiledCodeVert && codeSize > 0)
        {
            std::cout << "Shader compiled successfully! Code size: " << codeSize << " bytes\n";
        }

        const void* compiledCodeFrag = spGetEntryPointCode(Request, FragEntry, &codeSize);
        if (compiledCodeFrag && codeSize > 0)
        {
            std::cout << "Shader compiled successfully! Code size: " << codeSize << " bytes\n";
        }
    }

    spDestroyCompileRequest(Request);
}

}
