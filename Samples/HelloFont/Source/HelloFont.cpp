#include "HelloFont.h"
#include "Editor/Font.h"
#include "ResourceManager/ShaderManager.h"
#include "Runtime/AssetDatabase.h"
#include "Runtime/EntryPoint.h"
#include "Runtime/Font.h"

using namespace Nova;

HelloFont::HelloFont(const Array<const char*>& Arguments) : Application(Arguments)
{

}

ApplicationConfiguration HelloFont::CreateConfiguration() const
{
    ApplicationConfiguration Configuration;
    Configuration.AppName = "Hello Font | Nova Engine";
    Configuration.WindowWidth = 1280;
    Configuration.WindowHeight = 720;
    Configuration.WindowResizable = true;
    Configuration.Audio.SampleRate = 44100;
    Configuration.Audio.BufferSize = 1024;
    Configuration.Audio.BufferCount = 4;
    Configuration.Graphics.GraphicsApi = GraphicsApi::Vulkan;
    Configuration.Graphics.Buffering = SwapchainBuffering::TripleBuffering;
    Configuration.Graphics.VSync = true;
    Configuration.WithEditor = true;
    return Configuration;
}

void HelloFont::OnInit()
{
    Application::OnInit();

    ShaderManager* ShaderManager = GetShaderManager();
    const Path FontShader = PathCombine(Directory::GetApplicationDirectory(), "Shaders", "Font.slang");
    ShaderManager->Load("Font", FontShader);

    AssetDatabase* AssetDatabase = GetAssetDatabase();
    Font* FontAsset = AssetDatabase->CreateAsset<Font>("Font");
    const Path FontPath = GetFontPath(JetBrainsMono_Italic);
    const Array<CharacterSetRange> CharacterSetRanges = { { 0x0020, 0x00FF }};
    const FontParams FontParams { FontAtlasType::MTSDF, { ArrayView(CharacterSetRanges) }};
    if (!FontAsset->LoadFromFile(FontPath, FontParams))
    {
        RequireExit(ExitCode::Error);
        return;
    }

}

NOVA_DEFINE_APPLICATION_CLASS(HelloFont);
