#include "ArgumentParser.h"
#include "Containers/StringFormat.h"
#include "Runtime/Types.h"

namespace Nova
{
    ArgumentParserSettings ArgumentParserSettings::WindowsStyle = ArgumentParserSettings("/", "/", ':');
    ArgumentParserSettings ArgumentParserSettings::LinuxStyle = ArgumentParserSettings("-", "--", '=');

    std::regex ArgumentParserSettings::GetRegex() const
    {
        const String RegexString = Format(R"(^({}|{})[A-Za-z]+({}[A-Za-z0-9:/\\._]+)?$)", ShortFormatPrefix, LongFormatPrefix, AssigmentCharacter);
        return std::regex(*RegexString);
    }

    ArgumentParser::ArgumentParser(const String& Name, const Arguments& Arguments, const ArgumentParserSettings& Settings)
        : m_Name(Name), m_Args(Arguments), m_Settings(Settings)
    {
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
            String Values = Format("[{}]", Option.PossibleValues.Name);
            String Desc = Format("{}{} {}{} {}", m_Settings.ShortFormatPrefix, Option.ShortName, m_Settings.LongFormatPrefix, Option.LongName, Values);
            MaxChars = Math::Max(MaxChars, Desc.Count());

            for (const CommandLineOptionPossibleValue& PossibleValue : Option.PossibleValues.Values)
            {
                MaxChars = Math::Max(MaxChars, PossibleValue.Name.Count());
            }
        }

        return MaxChars;
    }

    size_t ArgumentParser::GetDescLength(const CommandLineOption& Option) const
    {
        const String Values = Format("[{}]", Option.PossibleValues.Name);
        const String Desc = Format("{}{} {}{} {}", m_Settings.ShortFormatPrefix, Option.ShortName, m_Settings.LongFormatPrefix, Option.LongName, Values);
        return Desc.Count();
    }

    String ArgumentParser::GetHelpText()
    {
        String HelpText;
        HelpText.Append(Format("Usage: {} [options]\n", m_Name));
        HelpText.Append("options:\n");

        i32 MaxDescLength = GetDescMaxLength(m_Options);

        for(const CommandLineOption& Option : m_Options)
        {
            String Values = Format("[{}]", Option.PossibleValues.Name);
            String Space;
            const i32 SpaceLength = MaxDescLength - GetDescLength(Option) + 1;
            for (i32 i = 0; i < SpaceLength; ++i)
                Space.Append(" ");
            String Required = Option.Required ? "(Required)" : "";
            String Formatted = Format("    {}{} {}{} {}{Space}\t{} {}\n",
                m_Settings.ShortFormatPrefix,
                Option.ShortName,
                m_Settings.
                LongFormatPrefix,
                Option.LongName,
                Values,
                Option.HelpText,
                Required);
            HelpText.Append(Formatted);
        }

        for (const CommandLineOption& Option : m_Options)
        {
            if (Option.PossibleValues == null) continue;
            HelpText.Append(Format("{}:", Option.PossibleValues.Name));

            for(const CommandLineOptionPossibleValue& PossibleValue : Option.PossibleValues.Values)
            {
                String Space;
                const i32 SpaceLength = MaxDescLength - PossibleValue.Name.Count() + 1;
                for (i32 i = 0; i < SpaceLength; i++)
                    Space.Append(" ");
                HelpText.Append(Format("    {}{}\t{}\n",
                    PossibleValue.Name,
                    Space,
                    PossibleValue.HelpText));
            }
        }
        return HelpText;
    }

    void ArgumentParser::AddOption(const CommandLineOption& Option)
    {
        m_Options.Add(Option);
    }


    void ArgumentParser::Parse()
    {
        if (m_Args.Count() <= 0)
            return;

        for (String::SizeType i = 0; i < m_Args.Count(); ++i)
        {
            String Argument = m_Args[i];
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

            if (m_ParsedArguments.Contains(Option) && !Option->AllowMultiple)
            {
                /*throw new InvalidCommandLineArgumentException($"Cannot use argument \"{Argument}\" multiple times.");*/
            }

            if(!m_ParsedArguments.Contains(Option))
                m_ParsedArguments[Option] = {};

            m_ParsedArguments[Option].Add(Split.Value);

        }

        const bool AllRequiredOptionsFound = m_Args.Any([this](const char* CurrentArg)
        {
            String OptionName = GetOptionNameFromArgument({CurrentArg});
            Array<String*> ShortNames = m_Options.Where([](const CommandLineOption& O) { return O.Required; }).Select<String>([](const CommandLineOption* O){ return O->ShortName; });
            Array<String*> LongNames = m_Options.Where([](const CommandLineOption& O) { return O.Required; }).Select<String>([](const CommandLineOption* O){ return O->LongName; });
            if (ShortNames.Count() == 0 && LongNames.Count() == 0) return true;
            return (ShortNames.Contains(OptionName[0]) && OptionName.Length <= 1) || LongNames.Contains(OptionName);
        });

        if (!AllRequiredOptionsFound)
        {
            //throw new InvalidCommandLineArgumentException("A required option was not found.");
        }
    }

    bool ArgumentParser::GetBool(CommandLineOption Option)
    {
        if (ParsedArguments.TryGetValue(Option, std::ios_base::out var Argument))
        {
            var Value = Argument[0];
            if (Value is bool BVal) return BVal;
            if (Value is String SVal) return !String.IsNullOrEmpty(SVal);
        }
        return false;
    }

    String ArgumentParser::GetString(CommandLineOption Option)
    {
        if (ParsedArguments.TryGetValue(Option, out var Argument))
        {
            var Value = Argument[0];
            if (Value is bool BVal) return BVal.ToString();
            if (Value is String SVal) return SVal;
        }
        return String.Empty;
    }

    Array<String> ArgumentParser::GetValues(CommandLineOption Option)
    {
        List<String> Result = new List<String>();
        if (ParsedArguments.TryGetValue(Option, out var Argument))
        {
            Argument.ForEach(S => Result.Add(S));
            return Result;
        }
        return null;
    }

    String ArgumentParser::GetOptionNameFromArgument(const String& Argument)
    {
        const String::SizeType AssignmentPosition = Argument.Find(m_Settings.AssigmentCharacter);
        String TrimmedArgument = Argument.TrimStart(PrefixCharacters.ToArray());
        return AssignmentPosition == -1 ? TrimmedArgument : TrimmedArgument.SubString(0, AssignmentPosition - 1);
    }

    Pair<String, std::any> ArgumentParser::SplitArgument(const String& Argument)
    {
        String Key;
        std::any Value;
        if (const String::SizeType AssignmentPosition = Argument.Find(m_Settings.AssigmentCharacter); AssignmentPosition == -1)
        {
            Key = Argument.TrimStart(PrefixCharacters.ToArray());
            Value = true;
        }
        else
        {
            String TrimmedKey = Argument.TrimStart(PrefixCharacters.ToArray());
            //Key = TrimmedKey[..(AssignmentPosition - 1)];
            //Value = Argument[(AssignmentPosition + 1)..];
        }

        return  { Key, Value };
    }

    bool ArgumentParser::IsArgumentValid(const String& Argument)
    {
        Array<String::CharacterType*> ShortNames = m_Options.Select<String::CharacterType>([](CommandLineOption& Option)
        {
            return &Option.ShortName;
        });

        Array<String*> LongNames = m_Options.Select<String>([](CommandLineOption& Option)
        {
            return &Option.LongName;
        });

        String OptionName = GetOptionNameFromArgument(Argument);
        std::regex Regex = m_Settings.GetRegex();

        return Settings.Regex.IsMatch(Argument) && ((ShortNames.Contains(OptionName[0]) && OptionName.Count() <= 1) || LongNames.Contains(OptionName));
    }
}
