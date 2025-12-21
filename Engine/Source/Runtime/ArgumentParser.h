#pragma once
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Containers/Map.h"
#include "CommandLineOption.h"
#include "CmdLineArgs.h"

#include <regex>
#include <any>


namespace Nova
{
    struct ArgumentParserSettings
    {
        String shortFormatPrefix;
        String longFormatPrefix;
        String::CharacterType assigmentCharacter;

        std::regex GetRegex() const;

        static ArgumentParserSettings DefaultStyle();
        static ArgumentParserSettings WindowsStyle();
        static ArgumentParserSettings LinuxStyle();
    };

    enum class ParsingResult
    {
        Success = 0,
        NoArgumentGiven,
        InvalidArgument,
        BadArgumentUsage,
        MissingRequiredOption
    };

    class ArgumentParser
    {
    public:
        explicit ArgumentParser(String name, const CmdLineArgs& args, const ArgumentParserSettings& settings);
        explicit ArgumentParser(String name, const CmdLineArgs& args, const ArgumentParserSettings& settings, const Array<CommandLineOption>& options);

        void AddOption(const CommandLineOption& option);
        void AddOptions(const Array<CommandLineOption>& options);

        ParsingResult Parse();

        bool GetBool(const CommandLineOption& option) const;
        bool GetBool(const String& optionName) const;
        bool GetBool(String::CharacterType optionName) const;

        String GetString(const CommandLineOption& option) const;
        String GetString(const String& optionName) const;
        String GetString(String::CharacterType optionName) const;

        Array<String> GetValues(const CommandLineOption& option) const;
        Array<String> GetValues(const String& optionName) const;
        Array<String> GetValues(String::CharacterType optionName) const;

        template<typename T> requires IsAnyOf<bool, String>
        T GetValueAs(const CommandLineOption& option) const
        {
            if constexpr (typeid(T) == typeid(bool))
                return GetBool(option);
            else if constexpr (typeid(T) == typeid(String))
                return GetString(option);
            return T(0);
        }

        template<typename T> requires IsAnyOf<bool, String>
        T GetValueAs(const String& optionName) const
        {
            if constexpr (typeid(T) == typeid(bool))
                return GetBool(optionName);
            else if constexpr (typeid(T) == typeid(String))
                return GetString(optionName);
            return T(0);
        }

        template<typename T> requires IsAnyOf<bool, String>
        T GetValueAs(String::CharacterType optionName) const
        {
            if constexpr (typeid(T) == typeid(bool))
                return GetBool(optionName);
            else if constexpr (typeid(T) == typeid(String))
                return GetString(optionName);
            return T(0);
        }

        String GetHelpText();
    private:
        bool IsArgumentValid(const String& argument) const;
        String GetOptionNameFromArgument(const String& argument) const;
        Pair<String, std::any> SplitArgument(const String& argument) const;
        size_t GetDescMaxLength(const Array<CommandLineOption>& options) const;
        size_t GetDescLength(const CommandLineOption& option) const;

        Array<String::CharacterType> GetPrefixCharacters() const;

    private:
        String m_Name;
        Array<StringView> m_Arguments;
        ArgumentParserSettings m_Settings;
        Map<CommandLineOption, Array<std::any>> m_ParsedArguments;
        Array<CommandLineOption> m_Options;
        Array<String::CharacterType> m_PrefixCharacters;
        std::regex m_Regex;
    };
}
