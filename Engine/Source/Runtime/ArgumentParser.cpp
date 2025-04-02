#include "ArgumentParser.h"
#include "Containers/StringFormat.h"

namespace Nova
{
    ArgumentParserSettings ArgumentParserSettings::WindowsStyle = ArgumentParserSettings("/", "/", ':');
    ArgumentParserSettings ArgumentParserSettings::LinuxStyle = ArgumentParserSettings("-", "--", '=');

    std::regex ArgumentParserSettings::GetRegex() const
    {
        const String RegexString = Format(R"(^({}|{})[A-Za-z]+({}[A-Za-z0-9:/\\._]+)?$)", ShortFormatPrefix, LongFormatPrefix, AssigmentCharacter);
        return std::regex(*RegexString);
    }

    ArgumentParser::ArgumentParser(const Arguments& Arguments, const ArgumentParserSettings& Settings)
        : m_Args(Arguments), m_Settings(Settings)
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
        StringBuilder Builder = new();
        Builder.AppendLine($"Usage: {Assembly.GetExecutingAssembly().GetName().Name} [options]");
        Builder.AppendLine("options:");


        int MaxDescLength = GetDescMaxLength(Options);

        foreach (CommandLineOption Option in Options)
        {
            string Values = Option.PossibleValues != null ? $"[{Option.PossibleValues.Name}]" : string.Empty;
            string Space = string.Empty;
            int SpaceLength = MaxDescLength - GetDescLength(Option) + 1;
            for (int i = 0; i < SpaceLength; i++) Space += " ";
            string Required = Option.Required ? "(Required)" : string.Empty;
            Builder.AppendLine($"    {Settings.ShortFormatPrefix}{Option.ShortName} {Settings.LongFormatPrefix}{Option.LongName} {Values}{Space}\t{Option.HelpText} {Required}");
        }

        foreach (CommandLineOption Option in Options)
        {
            if (Option.PossibleValues == null) continue;
            Builder.AppendLine($"{Option.PossibleValues.Name}:");
            foreach (CommandLineOptionPossibleValue PossibleValue in Option.PossibleValues.PossibleValues)
            {
                string Space = string.Empty;
                int SpaceLength = MaxDescLength - PossibleValue.Name.Length + 1;
                for (int i = 0; i < SpaceLength; i++) Space += " ";
                Builder.AppendLine($"    {PossibleValue.Name}{Space}\t{PossibleValue.HelpText}");
            }
        }
        return Builder.ToString();
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

            const auto Split = SplitArgument(Argument);
            CommandLineOption Option = Options.Single(O =>
                    (O.ShortName == SplittedArgument.Item1[0] && SplittedArgument.Item1.Length <= 1) ||
                    O.LongName == SplittedArgument.Item1);
            try
            {


                if (ParsedArguments.ContainsKey(Option) && !Option.AllowMultiple)
                    throw new InvalidCommandLineArgumentException($"Cannot use argument \"{Argument}\" multiple times.");

                if(!ParsedArguments.ContainsKey(Option))
                    ParsedArguments[Option] = [];
                ParsedArguments[Option].Add(SplittedArgument.Item2);

            }
            catch (InvalidOperationException)
            {
                throw new InvalidCommandLineArgumentException($"Argument \"{Argument}\" is not valid.");
            }


        }

        bool AllRequiredOptionsFound = Arguments.Any(A =>
        {
            string OptionName = GetOptionNameFromArgument(A);
            var ShortNames = Options.Where(O => O.Required).Select(O => O.ShortName).ToList();
            var LongNames = Options.Where(O => O.Required).Select(O => O.LongName).ToList();
            if (ShortNames.Count == 0 && LongNames.Count == 0) return true;
            return (ShortNames.Contains(OptionName[0]) && OptionName.Length <= 1) || LongNames.Contains(OptionName);
        });

        if (!AllRequiredOptionsFound)
        {
            throw new InvalidCommandLineArgumentException("A required option was not found.");
        }
    }

    bool ArgumentParser::GetBool(CommandLineOption Option)
    {
        if (ParsedArguments.TryGetValue(Option, std::ios_base::out var Argument))
        {
            var Value = Argument[0];
            if (Value is bool BVal) return BVal;
            if (Value is string SVal) return !string.IsNullOrEmpty(SVal);
        }
        return false;
    }

    String ArgumentParser::GetString(CommandLineOption Option)
    {
        if (ParsedArguments.TryGetValue(Option, out var Argument))
        {
            var Value = Argument[0];
            if (Value is bool BVal) return BVal.ToString();
            if (Value is string SVal) return SVal;
        }
        return string.Empty;
    }

    Array<String> ArgumentParser::GetValues(CommandLineOption Option)
    {
        List<string> Result = new List<string>();
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
        return AssignmentPosition == -1 ? TrimmedArgument : TrimmedArgument.Substring(0, AssignmentPosition - 1);
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
