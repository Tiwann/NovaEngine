#include "ArgumentParser.h"

#include <utility>
#include "Containers/StringFormat.h"
#include "Runtime/Types.h"
#include "Runtime/Assertion.h"
#include "Runtime/ArithmeticTypes.h"

namespace Nova
{
    ArgumentParserSettings ArgumentParserSettings::WindowsStyle = ArgumentParserSettings("/", "/", ':');
    ArgumentParserSettings ArgumentParserSettings::LinuxStyle = ArgumentParserSettings("-", "--", '=');

    std::regex ArgumentParserSettings::GetRegex() const
    {
        const String RegexString = StringFormat(R"(^({}|{})[A-Za-z]+({}[A-Za-z0-9:/\\._]+)?$)", ShortFormatPrefix, LongFormatPrefix, AssigmentCharacter);
        return std::regex(*RegexString);
    }

    ArgumentParser::ArgumentParser(String Name, Arguments  Arguments, ArgumentParserSettings  Settings)
        : m_Name(std::move(Name)), m_Arguments(std::move(Arguments)), m_Settings(std::move(Settings)), m_PrefixCharacters(GetPrefixCharacters())
    {
        m_Arguments.RemoveAt(0);
    }

    ArgumentParser::ArgumentParser(String  Name, const Arguments& Arguments,
        ArgumentParserSettings  Settings, const Array<CommandLineOption>& Options)
    : m_Name(std::move(Name)), m_Arguments(Arguments), m_Settings(std::move(Settings)), m_PrefixCharacters(GetPrefixCharacters()), m_Options(Options)
    {
        m_Arguments.RemoveAt(0);
        Parse();
    }

    void ArgumentParser::SetSettings(const ArgumentParserSettings& Settings)
    {
        m_Settings = Settings;
    }

    size_t ArgumentParser::GetDescMaxLength(const Array<CommandLineOption>& TheOptions) const
    {
        size_t MaxChars = 0;
        for (const CommandLineOption& Option : TheOptions)
        {
            String Values = Option.PossibleValues.has_value() ? StringFormat("[{}]", Option.PossibleValues.value().Name) : "";
            String Desc = StringFormat("{}{} {}{} {}", m_Settings.ShortFormatPrefix, Option.ShortName, m_Settings.LongFormatPrefix, Option.LongName, Values);
            MaxChars = Math::Max(MaxChars, Desc.Count());

            if (Option.PossibleValues.has_value())
            {
                for (const CommandLineOptionPossibleValue& PossibleValue : Option.PossibleValues.value().Values)
                {
                    MaxChars = Math::Max(MaxChars, PossibleValue.Name.Count());
                }
            }
        }

        return MaxChars;
    }

    size_t ArgumentParser::GetDescLength(const CommandLineOption& Option) const
    {
        const String Values = Option.PossibleValues.has_value() ? StringFormat("[{}]", Option.PossibleValues.value().Name) : "";
        const String Desc = StringFormat("{}{} {}{} {}", m_Settings.ShortFormatPrefix, Option.ShortName, m_Settings.LongFormatPrefix, Option.LongName, Values);
        return Desc.Count();
    }

    String ArgumentParser::GetHelpText()
    {
        String HelpText;
        HelpText.Append(StringFormat("Usage: {} [options]\n", m_Name));
        HelpText.Append("options:\n");

        const size_t MaxDescLength = GetDescMaxLength(m_Options);

        for(const CommandLineOption& Option : m_Options)
        {
            const String Values = Option.PossibleValues.has_value() ? StringFormat("[{}]", Option.PossibleValues.value().Name) : "";
            String Space;
            const size_t SpaceLength = MaxDescLength - GetDescLength(Option) + 1;
            for (size_t i = 0; i < SpaceLength; ++i)
                Space.Append(" ");
            String Required = Option.Required ? "(Required)" : "";
            String Formatted = StringFormat("    {}{} {}{} {}{}\t{} {}\n",
                m_Settings.ShortFormatPrefix,
                Option.ShortName,
                m_Settings.LongFormatPrefix,
                Option.LongName,
                Values,
                Space,
                Option.HelpText,
                Required);
            HelpText.Append(Formatted);
        }

        for (const CommandLineOption& Option : m_Options)
        {
            if (!Option.PossibleValues.has_value()) continue;
            HelpText.Append(StringFormat("{}:", Option.PossibleValues.value().Name));

            for(const CommandLineOptionPossibleValue& PossibleValue : Option.PossibleValues.value().Values)
            {
                String Space;
                const i32 SpaceLength = MaxDescLength - PossibleValue.Name.Count() + 1;
                for (i32 i = 0; i < SpaceLength; i++) Space.Append(" ");
                HelpText.Append(StringFormat("    {}{}\t{}\n",PossibleValue.Name,Space,PossibleValue.HelpText));
            }
        }
        return HelpText;
    }

    Array<String::CharacterType> ArgumentParser::GetPrefixCharacters() const
    {
        return m_Settings.ShortFormatPrefix.ToArray().Union(m_Settings.LongFormatPrefix.ToArray());
    }

    void ArgumentParser::AddOption(const CommandLineOption& Option)
    {
        m_Options.Add(Option);
    }

    void ArgumentParser::AddOptions(const Array<CommandLineOption>& Options)
    {
        m_Options.AddRange(Options);
    }


    void ArgumentParser::Parse()
    {
        if (m_Arguments.Count() <= 0)
            return;

        for (String::SizeType i = 0; i < m_Arguments.Count(); ++i)
        {
            String Argument = m_Arguments[i];
            if (!IsArgumentValid(Argument))
            {
                //throw new InvalidCommandLineArgumentException($"Argument \"{Argument}\" is not valid.");
                return;
            }

            const Pair<String, std::any> Split = SplitArgument(Argument);

            CommandLineOption* Option = m_Options.Single([&Split](const CommandLineOption& Option)
            {
                return Option.ShortName == Split.Key[0] && Split.Key.Count() <= 1 ||
                    Option.LongName == Split.Key;
            });

            if (m_ParsedArguments.Contains(*Option) && !Option->AllowMultiple)
            {
                /*throw new InvalidCommandLineArgumentException($"Cannot use argument \"{Argument}\" multiple times.");*/
            }

            if(!m_ParsedArguments.Contains(*Option))
                m_ParsedArguments[*Option] = {};

            m_ParsedArguments[*Option].Add(Split.Value);

        }

        const bool AllRequiredOptionsFound = m_Arguments.Any([this](const char* CurrentArg)
        {
            String OptionName = GetOptionNameFromArgument(String(CurrentArg));
            Array<const String::CharacterType*> ShortNames = m_Options.Where([](const CommandLineOption& O) { return O.Required; }).Select<const String::CharacterType>([](const CommandLineOption* O){ return &O->ShortName; });
            Array<const String*> LongNames = m_Options.Where([](const CommandLineOption& O) { return O.Required; }).Select<const String>([](const CommandLineOption* O){ return &O->LongName; });
            if (ShortNames.Count() == 0 && LongNames.Count() == 0) return true;

            bool ShortNameCheck = false;
            for (const String::CharacterType* ShortName : ShortNames)
            {
                if (*ShortName == OptionName[0] && OptionName.Count() <= 1)
                {
                    ShortNameCheck = true;
                    break;
                }
            }

            bool LongNameCheck = false;
            for (const String* LongName : LongNames)
            {
                if (LongName->Find(OptionName) != -1)
                {
                    LongNameCheck = true;
                    break;
                }
            }
            return ShortNameCheck || LongNameCheck;
        });

        if (!AllRequiredOptionsFound)
        {
            //throw new InvalidCommandLineArgumentException("A required option was not found.");
        }
    }

    bool ArgumentParser::GetBool(const CommandLineOption& Option)
    {
        if (m_ParsedArguments.Contains(Option))
        {
            const Array<std::any>& Argument = m_ParsedArguments[Option];
            const std::any& Value = Argument[0];
            if (Value.type() == typeid(bool))
                return std::any_cast<bool>(Value);

            if (Value.type() == typeid(String))
                return !std::any_cast<String>(Value).IsEmpty();
        }
        return false;
    }

    static String BoolToString(const bool Value)
    {
        return Value ? "true" : "false";
    }

    String ArgumentParser::GetString(const CommandLineOption& Option)
    {
        if (m_ParsedArguments.Contains(Option))
        {
            const Array<std::any>& Argument = m_ParsedArguments[Option];
            const std::any& Value = Argument[0];
            if (Value.type() == typeid(bool))
                return BoolToString(std::any_cast<bool>(Value));
            if (Value.type() == typeid(String))
                return std::any_cast<String>(Value);
        }
        return "";
    }

    Array<String> ArgumentParser::GetValues(const CommandLineOption& Option)
    {
        Array<String> Result;
        if (m_ParsedArguments.Contains(Option))
        {
            for (const Array<std::any>& Argument = m_ParsedArguments[Option]; const std::any& Value : Argument)
            {
                Result.Add(std::any_cast<String>(Value));
            }
        }
        return Result;
    }

    String ArgumentParser::GetOptionNameFromArgument(const String& Argument) const
    {
        const String::SizeType AssignmentPosition = Argument.Find(m_Settings.AssigmentCharacter);
        String TrimmedArgument = Argument.TrimStart(m_PrefixCharacters);
        return AssignmentPosition == -1 ? TrimmedArgument : TrimmedArgument.Substring(0, AssignmentPosition - 1);
    }

    Pair<String, std::any> ArgumentParser::SplitArgument(const String& Argument) const
    {
        String Key;
        std::any Value;
        if (const String::SizeType AssignmentPosition = Argument.Find(m_Settings.AssigmentCharacter); AssignmentPosition == -1)
        {
            Key = Argument.TrimStart(m_PrefixCharacters);
            Value = true;
        }
        else
        {
            String TrimmedKey = Argument.TrimStart(m_PrefixCharacters);
            //Key = TrimmedKey[..(AssignmentPosition - 1)];
            //Value = Argument[(AssignmentPosition + 1)..];
        }

        return  { Key, Value };
    }

    bool ArgumentParser::IsArgumentValid(const String& Argument) const
    {
        Array<const String::CharacterType*> ShortNames = m_Options.Select<const String::CharacterType>([](const CommandLineOption& Option){ return &Option.ShortName;});
        Array<const String*> LongNames = m_Options.Select<const String>([](const CommandLineOption& Option){ return &Option.LongName; });
        String OptionName = GetOptionNameFromArgument(Argument);
        bool ShortNameCheck = false;

        for (const String::CharacterType* ShortName : ShortNames)
        {
            if (*ShortName == OptionName[0] && OptionName.Count() <= 1)
            {
                ShortNameCheck = true;
                break;
            }
        }

        bool LongNameCheck = false;
        for (const String* LongName : LongNames)
        {
            if (LongName->Find(OptionName) != -1)
            {
                LongNameCheck = true;
                break;
            }
        }

        const std::regex Regex = m_Settings.GetRegex();
        return std::regex_match(*Argument, Regex) && (ShortNameCheck || LongNameCheck);
    }
}
