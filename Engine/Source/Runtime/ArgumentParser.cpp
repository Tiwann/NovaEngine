#include "ArgumentParser.h"
#include "Containers/StringFormat.h"
#include <utility>

namespace Nova
{
    std::regex ArgumentParserSettings::GetRegex() const
    {
        const String string = StringFormat(R"(^({}|{})[A-Za-z]+({}[A-Za-z0-9:/\\._]+)?$)", shortFormatPrefix, longFormatPrefix, assigmentCharacter);
        return std::regex(*string);
    }

    ArgumentParserSettings ArgumentParserSettings::DefaultStyle()
    {
#ifdef NOVA_PLATFORM_WINDOWS
        return WindowsStyle();
#else
        return LinuxStyle();
#endif
    }

    ArgumentParserSettings ArgumentParserSettings::WindowsStyle()
    {
        return ArgumentParserSettings("/", "/", ':');
    }

    ArgumentParserSettings ArgumentParserSettings::LinuxStyle()
    {
        return ArgumentParserSettings("-", "--", '=');
    }

    ArgumentParser::ArgumentParser(String name, const CmdLineArgs& args, const ArgumentParserSettings& settings)
        : m_Name(std::move(name)), m_Settings(settings), m_PrefixCharacters(GetPrefixCharacters())
    {
        for (size_t i = 1; i < args.Count(); i++)
            m_Arguments.Add(args.GetArgument(i));
    }

    ArgumentParser::ArgumentParser(String name, const CmdLineArgs& args, const ArgumentParserSettings& settings, const Array<CommandLineOption>& options)
    : m_Name(std::move(name)), m_Settings(settings), m_Options(options), m_PrefixCharacters(GetPrefixCharacters())
    {
        for (size_t i = 1; i < args.Count(); i++)
            m_Arguments.Add(args.GetArgument(i));
    }


    size_t ArgumentParser::GetDescMaxLength(const Array<CommandLineOption>& options) const
    {
        size_t maxChars = 0;
        for (const CommandLineOption& option : options)
        {
            String values = option.possibleValues.has_value() ? StringFormat("[{}]", option.possibleValues.value().name) : "";
            String desc = StringFormat("{}{} {}{} {}", m_Settings.shortFormatPrefix, option.shortName, m_Settings.longFormatPrefix, option.longName, values);
            maxChars = Math::Max(maxChars, desc.Count());

            if (option.possibleValues.has_value())
            {
                for (const CommandLineOptionPossibleValue& possibleValue : option.possibleValues.value().values)
                {
                    maxChars = Math::Max(maxChars, possibleValue.name.Count());
                }
            }
        }

        return maxChars;
    }

    size_t ArgumentParser::GetDescLength(const CommandLineOption& option) const
    {
        const String values = option.possibleValues.has_value() ? StringFormat("[{}]", option.possibleValues.value().name) : "";
        const String desc = StringFormat("{}{} {}{} {}", m_Settings.shortFormatPrefix, option.shortName, m_Settings.longFormatPrefix, option.longName, values);
        return desc.Count();
    }

    String ArgumentParser::GetHelpText()
    {
        String helpText;
        helpText.Append(StringFormat("Usage: {} [options]\n", m_Name));
        helpText.Append("options:\n");

        const size_t maxDescLength = GetDescMaxLength(m_Options);

        for(const CommandLineOption& Option : m_Options)
        {
            const String values = Option.possibleValues.has_value() ? StringFormat("[{}]", Option.possibleValues.value().name) : "";
            String space;

            const size_t spaceLength = maxDescLength - GetDescLength(Option) + 1;
            for (size_t i = 0; i < spaceLength; ++i)
                space.Append(' ');

            String required = Option.required ? "(required)" : "";
            String formatted = StringFormat("    {}{} {}{} {}{}\t{} {}\n",
                m_Settings.shortFormatPrefix,
                Option.shortName,
                m_Settings.longFormatPrefix,
                Option.longName,
                values,
                space,
                Option.helpText,
                required);
            helpText.Append(formatted);
        }

        for (const CommandLineOption& option : m_Options)
        {
            if (!option.possibleValues.has_value()) continue;
            helpText.Append(StringFormat("{}:", option.possibleValues.value().name));

            for(const CommandLineOptionPossibleValue& possibleValue : option.possibleValues.value().values)
            {
                String space;
                const int32_t spaceLength = maxDescLength - possibleValue.name.Count() + 1;
                for (int32_t i = 0; i < spaceLength; i++)
                    space.Append(' ');
                helpText.Append(StringFormat("    {}{}\t{}\n", possibleValue.name, space, possibleValue.helpText));
            }
        }
        return helpText;
    }

    Array<String::CharacterType> ArgumentParser::GetPrefixCharacters() const
    {
        return m_Settings.shortFormatPrefix.AsArray().Union(m_Settings.longFormatPrefix.AsArray());
    }

    void ArgumentParser::AddOption(const CommandLineOption& option)
    {
        m_Options.Add(option);
    }

    void ArgumentParser::AddOptions(const Array<CommandLineOption>& options)
    {
        m_Options.AddRange(options);
    }


    ParsingResult ArgumentParser::Parse()
    {
        if (m_Arguments.Count() <= 0)
            return ParsingResult::NoArgumentGiven;

        for (String::SizeType i = 0; i < m_Arguments.Count(); ++i)
        {
            String argument = String(*m_Arguments[i]);
            if (!IsArgumentValid(argument))
                return ParsingResult::InvalidArgument;

            const Pair<String, std::any> split = SplitArgument(argument);

            CommandLineOption* option = m_Options.Single([&split](const CommandLineOption& Option)
            {
                return Option.shortName == split.key[0] && split.key.Count() <= 1 ||
                    Option.longName == split.key;
            });

            if (!option) continue;

            if (m_ParsedArguments.Contains(*option) && !option->allowMultiple)
            {
                return ParsingResult::BadArgumentUsage;
            }

            if(!m_ParsedArguments.Contains(*option))
                m_ParsedArguments[*option] = {};

            m_ParsedArguments[*option].Add(split.value);

        }

        const bool allRequiredOptionsFound = m_Arguments.Any([this](StringView currentArg)
        {
            String optionName = GetOptionNameFromArgument(String(currentArg));
            Array<const String::CharacterType*> shortNames = m_Options.Where([](const CommandLineOption& O) { return O.required; }).Select<const String::CharacterType>([](const CommandLineOption* O){ return &O->shortName; });
            Array<const String*> longNames = m_Options.Where([](const CommandLineOption& O) { return O.required; }).Select<const String>([](const CommandLineOption* O){ return &O->longName; });
            if (shortNames.Count() == 0 && longNames.Count() == 0) return true;

            bool shortNameCheck = false;
            for (const String::CharacterType* shortName : shortNames)
            {
                if (*shortName == optionName[0] && optionName.Count() <= 1)
                {
                    shortNameCheck = true;
                    break;
                }
            }

            bool longNameCheck = false;
            for (const String* longName : longNames)
            {
                if (longName->Find(optionName) != -1)
                {
                    longNameCheck = true;
                    break;
                }
            }
            return shortNameCheck || longNameCheck;
        });

        if (!allRequiredOptionsFound)
            return ParsingResult::MissingRequiredOption;
        return ParsingResult::Success;
    }

    static String BoolToString(const bool value)
    {
        return value ? "true" : "false";
    }

    bool ArgumentParser::GetBool(const CommandLineOption& option)
    {
        if (m_ParsedArguments.Contains(option))
        {
            const Array<std::any>& argument = m_ParsedArguments[option];
            const std::any& value = argument[0];
            if (value.type() == typeid(bool))
                return std::any_cast<bool>(value);

            if (value.type() == typeid(String))
                return !std::any_cast<String>(value).IsEmpty();
        }
        return false;
    }

    bool ArgumentParser::GetBool(const String& optionName)
    {
        for (Pair<CommandLineOption, Array<std::any>>& parsedArgument : m_ParsedArguments)
        {
            CommandLineOption& option = parsedArgument.key;
            Array<std::any>& values = parsedArgument.value;

            if (option.longName != optionName)
                continue;

            const std::any& value = values.First();
            if (value.type() == typeid(bool))
                return std::any_cast<bool>(value);

            if (value.type() == typeid(String))
                return !std::any_cast<String>(value).IsEmpty();
        }

        return false;
    }

    bool ArgumentParser::GetBool(String::CharacterType optionName)
    {
        for (Pair<CommandLineOption, Array<std::any>>& parsedArgument : m_ParsedArguments)
        {
            CommandLineOption& option = parsedArgument.key;
            Array<std::any>& values = parsedArgument.value;

            if (option.shortName != optionName)
                continue;

            const std::any& value = values.First();
            if (value.type() == typeid(bool))
                return std::any_cast<bool>(value);

            if (value.type() == typeid(String))
                return !std::any_cast<String>(value).IsEmpty();
        }

        return false;
    }

    String ArgumentParser::GetString(const CommandLineOption& option)
    {
        if (m_ParsedArguments.Contains(option))
        {
            const Array<std::any>& argument = m_ParsedArguments[option];
            const std::any& value = argument[0];
            if (value.type() == typeid(bool))
                return BoolToString(std::any_cast<bool>(value));
            if (value.type() == typeid(String))
                return std::any_cast<String>(value);
        }
        return "";
    }

    String ArgumentParser::GetString(const String& optionName)
    {
        for (Pair<CommandLineOption, Array<std::any>>& parsedArgument : m_ParsedArguments)
        {
            CommandLineOption& option = parsedArgument.key;
            Array<std::any>& values = parsedArgument.value;

            if (option.longName != optionName)
                continue;

            const std::any& value = values.First();
            if (value.type() == typeid(bool))
                return BoolToString(std::any_cast<bool>(value));

            if (value.type() == typeid(String))
                return std::any_cast<String>(value);
        }

        return "";
    }

    String ArgumentParser::GetString(String::CharacterType optionName)
    {
        for (Pair<CommandLineOption, Array<std::any>>& parsedArgument : m_ParsedArguments)
        {
            CommandLineOption& option = parsedArgument.key;
            Array<std::any>& values = parsedArgument.value;

            if (option.shortName != optionName)
                continue;

            const std::any& value = values.First();
            if (value.type() == typeid(bool))
                return BoolToString(std::any_cast<bool>(value));

            if (value.type() == typeid(String))
                return std::any_cast<String>(value);
        }

        return "";
    }

    Array<String> ArgumentParser::GetValues(const CommandLineOption& option)
    {
        Array<String> result;
        if (m_ParsedArguments.Contains(option))
        {
            for (const Array<std::any>& argument = m_ParsedArguments[option]; const std::any& value : argument)
            {
                result.Add(std::any_cast<String>(value));
            }
        }
        return result;
    }

    Array<String> ArgumentParser::GetValues(const String& optionName)
    {
        Array<String> result;
        for (Pair<CommandLineOption, Array<std::any>>& parsedArgument : m_ParsedArguments)
        {
            CommandLineOption& option = parsedArgument.key;
            Array<std::any>& values = parsedArgument.value;

            if (option.longName != optionName)
                continue;

            for (const std::any& value : values)
                result.Add(std::any_cast<String>(value));
        }
        return result;
    }

    Array<String> ArgumentParser::GetValues(String::CharacterType optionName)
    {
        Array<String> result;
        for (Pair<CommandLineOption, Array<std::any>>& parsedArgument : m_ParsedArguments)
        {
            CommandLineOption& option = parsedArgument.key;
            Array<std::any>& values = parsedArgument.value;

            if (option.shortName != optionName)
                continue;

            for (const std::any& value : values)
                result.Add(std::any_cast<String>(value));
        }
        return result;
    }

    String ArgumentParser::GetOptionNameFromArgument(const String& argument) const
    {
        const String::SizeType assignmentPos = argument.Find(m_Settings.assigmentCharacter);
        String trimmedArgument = argument.TrimStart(m_PrefixCharacters);
        return assignmentPos == -1 ? trimmedArgument : trimmedArgument.Substring(0, assignmentPos - 2);
    }

    Pair<String, std::any> ArgumentParser::SplitArgument(const String& argument) const
    {
        String key;
        std::any value;
        const String arg = argument.TrimStart(m_PrefixCharacters);
        const String::SizeType assignmentPosition = arg.Find(m_Settings.assigmentCharacter);
        if (assignmentPosition == -1)
        {
            key = arg;
            value = true;
        }
        else
        {
            key = arg.Substring(0, assignmentPosition - 1);
            value = arg.Substring(assignmentPosition + 1);
        }

        return  { key, value };
    }

    bool ArgumentParser::IsArgumentValid(const String& argument) const
    {
        Array<const String::CharacterType*> shortNames = m_Options.Select<const String::CharacterType>([](const CommandLineOption& option){ return &option.shortName;});
        Array<const String*> longNames = m_Options.Select<const String>([](const CommandLineOption& option){ return &option.longName; });
        String optionName = GetOptionNameFromArgument(argument);
        bool shortNameCheck = false;

        for (const String::CharacterType* shortName : shortNames)
        {
            if (!shortNameCheck) continue;
            if (*shortName == optionName[0] && optionName.Count() <= 1)
            {
                shortNameCheck = true;
                break;
            }
        }

        bool longNameCheck = false;
        for (const String* longName : longNames)
        {
            if (!longName) continue;
            if (longName->Find(optionName) != -1)
            {
                longNameCheck = true;
                break;
            }
        }

        const std::regex regex = m_Settings.GetRegex();
        return std::regex_match(*argument, regex) && (shortNameCheck || longNameCheck);
    }
}
