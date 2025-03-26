using System.Collections.ObjectModel;
using System.Reflection;
using System.Text;
using Microsoft.CSharp.RuntimeBinder;

namespace NovaMake;

/// <summary>
/// Provides utilities to parse command line arguments
/// </summary>
public class ArgumentParser(IEnumerable<string> Arguments, ArgumentParserSettings Settings)
{
    public ArgumentParserSettings Settings { get; } = Settings;

    private readonly Dictionary<CommandLineOption, List<dynamic>> ParsedArguments = new();
    private Collection<CommandLineOption> Options = [];
    private readonly IEnumerable<char> PrefixCharacters = Settings.ShortFormatPrefix.ToCharArray().Union(Settings.LongFormatPrefix.ToCharArray());

    private bool IsArgumentValid(string Argument)
    {
        var ShortNames = Options.Select(O => O.ShortName);
        var LongNames = Options.Select(O => O.LongName);
        string OptionName = GetOptionNameFromArgument(Argument);
        return Settings.Regex.IsMatch(Argument) && ((ShortNames.Contains(OptionName[0]) && OptionName.Length <= 1) || LongNames.Contains(OptionName));
    }

    private string GetOptionNameFromArgument(string Argument)
    {
        int AssignmentPosition = Argument.IndexOf(Settings.AssigmentCharacter);
        string TrimmedArgument = Argument.TrimStart(PrefixCharacters.ToArray());
        return AssignmentPosition == -1 ? TrimmedArgument : TrimmedArgument.Substring(0, AssignmentPosition - 1);
    }

    private (string, dynamic) SplitArgument(string Argument)
    {
        string Key;
        dynamic Value;
        int AssignmentPosition = Argument.IndexOf(Settings.AssigmentCharacter);
        if (AssignmentPosition == -1)
        {
            Key = Argument.TrimStart(PrefixCharacters.ToArray());
            Value = true;
        }
        else
        {
            string TrimmedKey = Argument.TrimStart(PrefixCharacters.ToArray());
            Key = TrimmedKey[..(AssignmentPosition - 1)];
            Value = Argument[(AssignmentPosition + 1)..];
        }

        return (Key, Value);
    }

    [Obsolete]
    public void AddOptions(params CommandLineOption[] InOptions)
    {
        Options = new Collection<CommandLineOption>(InOptions);
    }

    public void AddOptions(Type ClassType)
    {
        var OptionsAttribute = ClassType.GetCustomAttribute<ArgumentParserOptionsAttribute>();
        if (OptionsAttribute is null)
            throw new NullReferenceException($"Class {ClassType.Name} should have a ArgumentParserOptionsAttribute.");

        IEnumerable<CommandLineOption> CmdLineOptions = ClassType.GetFields()
            .Where(Info => Info.IsPublic && Info.IsStatic).Select(Info => Info.GetValue(null) as CommandLineOption);
        Options = [..CmdLineOptions];
    }

    public void AddOptions<T>() => AddOptions(typeof(T));
    
    public void Parse()
    {
        if (!Arguments.Any())
            throw new NoArgumentProvidedException($"No arguments were provided. Try {Assembly.GetExecutingAssembly().GetName().Name} {Settings.LongFormatPrefix}help.");
        
        foreach (string Argument in Arguments)
        {
            // Throw if argument isn't valid
            if (!IsArgumentValid(Argument)) throw new InvalidCommandLineArgumentException($"Argument \"{Argument}\" is not valid.");
            
            (string, dynamic) SplittedArgument = SplitArgument(Argument);
            try
            {
                CommandLineOption Option = Options.Single(O =>
                    (O.ShortName == SplittedArgument.Item1[0] && SplittedArgument.Item1.Length <= 1) ||
                    O.LongName == SplittedArgument.Item1);
                
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


    private int GetDescMaxLength(IEnumerable<CommandLineOption> TheOptions)
    {
        int MaxChars = 0;
        foreach (CommandLineOption Option in TheOptions)
        {
            string Values = Option.PossibleValues != null ? $"[{Option.PossibleValues.Name}]" : string.Empty;
            string Desc = $"{Settings.ShortFormatPrefix}{Option.ShortName} {Settings.LongFormatPrefix}{Option.LongName} {Values}";
            MaxChars = Math.Max(MaxChars, Desc.Length);
            if (Option.PossibleValues != null)
            {
                foreach (CommandLineOptionPossibleValue PossibleValue in Option.PossibleValues.PossibleValues)
                {
                    MaxChars = Math.Max(MaxChars, PossibleValue.Name.Length);
                } 
            }
        }

        return MaxChars;
    }

    private int GetDescLength(CommandLineOption Option)
    {
        string Values = Option.PossibleValues != null ? $"[{Option.PossibleValues.Name}]" : string.Empty;
        string Desc = $"{Settings.ShortFormatPrefix}{Option.ShortName} {Settings.LongFormatPrefix}{Option.LongName} {Values}";
        return Desc.Length;
    }
    
    public string GetHelpText()
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
    
    public bool GetBool(CommandLineOption Option)
    {
        if (ParsedArguments.TryGetValue(Option, out var Argument))
        {
            var Value = Argument[0];
            if (Value is bool BVal) return BVal;
            if (Value is string SVal) return !string.IsNullOrEmpty(SVal);
        }
        return false;
    }

    public string GetString(CommandLineOption Option)
    {
        if (ParsedArguments.TryGetValue(Option, out var Argument))
        {
            var Value = Argument[0];
            if (Value is bool BVal) return BVal.ToString();
            if (Value is string SVal) return SVal;
        }
        return string.Empty;
    }

    public dynamic GetValue<T>(CommandLineOption Option)
    {
        try
        {
            if (ParsedArguments.TryGetValue(Option, out var Argument))
            {
                T Value = Argument[0];
                return Value;
            }
        }
        catch (RuntimeBinderException)
        {
            Log.Error($"Option \"{Option.LongName}\" is not being used well. Try {Assembly.GetExecutingAssembly().GetName().Name} {Settings.LongFormatPrefix}{NovaMake.Options.Help.LongName} to show help");
            Environment.Exit(0);
        }

        return false;
    }

    public IEnumerable<string> GetValues(CommandLineOption Option)
    {
        List<string> Result = new List<string>();
        if (ParsedArguments.TryGetValue(Option, out var Argument))
        {
            Argument.ForEach(S => Result.Add(S));
            return Result;
        }
        return null;
    }
}
