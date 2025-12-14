#include "AssetPackerApplication.h"
#include "Runtime/ArgumentParser.h"
#include "Containers/StringFormat.h"
#include <print>

#include "Runtime/Path.h"

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

    void AssetPackerApplication::OnInit()
    {
        const CmdLineArgs& args = GetProgramArguments();
        const ArgumentParserSettings parserSettings = ArgumentParserSettings::DefaultStyle();

        CommandLineOption fileOption = {'f', "file", false, true, "Add a file to the asset list"};
        CommandLineOption directoryOption = {'d', "directory", false, true, "Add a directory to the asset list"};
        CommandLineOption versionOption = {'v', "version", false, false, "Specify the .nepack version to use"};
        CommandLineOption compressOption = {'c', "comp", false, false, "Specify wether to compress the assets or not"};

        ArgumentParser parser("AssetPacker", args, parserSettings);
        parser.AddOptions({fileOption, directoryOption, versionOption, compressOption});

        ParsingResult result = parser.Parse();
        if (result != ParsingResult::Success)
        {
            String helpText = parser.GetHelpText();
            std::cout << helpText << std::endl;
            Exit();
        }

        Array<String> assetPaths;

        Array<String> files = parser.GetValues(fileOption);
        Array<String> directories = parser.GetValues(directoryOption);

        for (const String& file : files)
            if (Path::Exists(file))
                assetPaths.Add(file);
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
