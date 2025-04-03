#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Containers/Map.h"
#include "CommandLineOption.h"
#include <CoreExport.h>

#include <regex>
#include <any>

namespace Nova
{
    struct ArgumentParserSettings
    {
        String ShortFormatPrefix;
        String LongFormatPrefix;
        String::CharacterType AssigmentCharacter;

        std::regex GetRegex() const;

        static ArgumentParserSettings WindowsStyle;
        static ArgumentParserSettings LinuxStyle;
    };

    class CORE_API ArgumentParser
    {
    public:
        using Arguments = Array<const char*>;
        explicit ArgumentParser(const String& Name, const Arguments& Arguments, const ArgumentParserSettings& Settings);


        void SetSettings(const ArgumentParserSettings& Settings);
        void AddOption(const CommandLineOption& Option);
        void AddOptions(const Array<CommandLineOption>& Options);
        void Parse();
        bool GetBool(const CommandLineOption& Option);
        String GetString(const CommandLineOption& Option);
        Array<String> GetValues(const CommandLineOption& Option);

        /*template <typename T>
        std::any GetValue<T>(CommandLineOption Option)
        {
            try
            {
                if (ParsedArguments.TryGetValue(Option, out var Argument))
                {
                    T Value = Argument[0];
                    return {Value};
                }
            }
            catch (RuntimeBinderException)
            {
                Log.Error(
                    $"Option \"{Option.LongName}\" is not being used well. Try {Assembly.GetExecutingAssembly().GetName().Name} {Settings.LongFormatPrefix}{NovaMake.Options.Help.LongName} to show help");
                Environment.Exit(0);
            }

            return {false};
        }*/

    private:
        bool IsArgumentValid(const String& Argument) const;
        String GetOptionNameFromArgument(const String& Argument) const;
        Pair<String, std::any> SplitArgument(const String& Argument) const;
        size_t GetDescMaxLength(const Array<CommandLineOption>& TheOptions) const;
        size_t GetDescLength(const CommandLineOption& Option) const;
        String GetHelpText();

        Array<String::CharacterType> GetPrefixCharacters() const;

    private:
        String m_Name;
        Arguments m_Arguments;
        ArgumentParserSettings m_Settings;
        Map<CommandLineOption, Array<std::any>> m_ParsedArguments;
        Array<CommandLineOption> m_Options;
        Array<String::CharacterType> m_PrefixCharacters;
        std::regex m_Regex;
    };
}
