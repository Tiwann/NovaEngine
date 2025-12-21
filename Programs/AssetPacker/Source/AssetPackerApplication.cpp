#include "AssetPackerApplication.h"
#include "Runtime/ArgumentParser.h"
#include "Runtime/Path.h"
#include "Runtime/LogCategory.h"
#include "Runtime/Log.h"

NOVA_DECLARE_LOG_CATEGORY_STATIC(AssetPacker, "AssetPacker")

namespace Nova
{
    extern "C" Application* CreateApplication(const int argc, char** argv)
    {
        return new AssetPackerApplication(argc, argv);
    }

    ApplicationConfiguration AssetPackerApplication::GetConfiguration() const
    {
        ApplicationConfiguration config = {};
        config.applicationName = "Nova Asset Packer";
        return config;
    }

    static Array<String> GetAssetList(const ArgumentParser& parser)
    {
        Array<String> assetList;

        Array<String> files = parser.GetValues('f');
        Array<String> dirs = parser.GetValues('d');

        for (const String& file : files)
            if (Path::Exists(file))
                assetList.Add(file);

        for (const String& dir : dirs)
            assetList.AddRange(Path::GetFiles(dir));

        return assetList;
    }

    void AssetPackerApplication::OnInit()
    {
        const CmdLineArgs& args = GetProgramArguments();
        const ArgumentParserSettings parserSettings = ArgumentParserSettings::LinuxStyle();

        CommandLineOption fileOption = {'f', "file", false, true, "Add a file to the asset list"};
        CommandLineOption directoryOption = {'d', "directory", false, true, "Add a directory to the asset list"};
        CommandLineOption outputOption = {'o', "output", true, false, "Specify the output asset pack file"};
        CommandLineOption projectOption = {'p', "project", false, false, "Specify a project directory"};

        const StringView engineDir = Path::GetEngineDirectory();
        ArgumentParser parser("AssetPacker", args, parserSettings);
        parser.AddOptions({fileOption, directoryOption, outputOption});

        ParsingResult result = parser.Parse();
        if (result != ParsingResult::Success)
        {
            NOVA_LOG(AssetPacker, Verbosity::Trace, parser.GetHelpText());
            Exit();
        }

        String projectDir = parser.GetString('p');

    }

    void AssetPackerApplication::OnDestroy()
    {
        Application::OnDestroy();
    }

    DeviceType AssetPackerApplication::GetRenderDeviceType() const
    {
        return DeviceType::Vulkan;
    }
}
