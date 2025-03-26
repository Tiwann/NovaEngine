namespace NovaMake;

public class CommandLineOption
{
    public char ShortName { get; }
    public string LongName { get; }
    public bool Required { get; }
    public bool AllowMultiple { get; }
    public string HelpText { get; }
    public CommandLineOptionPossibleValueList PossibleValues { get; }
    
    public CommandLineOption(char ShortName, string LongName, bool Required, bool AllowMultiple, CommandLineOptionPossibleValueList PossibleValues = null)
    {
        this.ShortName = ShortName;
        this.LongName = LongName;
        this.Required = Required;
        this.AllowMultiple = AllowMultiple;
        this.PossibleValues = PossibleValues;
    }
    
    public CommandLineOption(char ShortName, string LongName, bool Required, bool AllowMultiple, string HelpText, CommandLineOptionPossibleValueList PossibleValues = null)
    {
        this.ShortName = ShortName;
        this.LongName = LongName;
        this.Required = Required;
        this.AllowMultiple = AllowMultiple;
        this.HelpText = HelpText;
        this.PossibleValues = PossibleValues;
    }
}

public class CommandLineOptionPossibleValue
{
    public string Name { get; }
    public string HelpText { get; }

    public CommandLineOptionPossibleValue(string Name, string HelpText)
    {
        this.Name = Name;
        this.HelpText = HelpText;
    }
}

public class CommandLineOptionPossibleValueList
{
    public string Name { get; }
    public CommandLineOptionPossibleValue[] PossibleValues { get; }
    
    public CommandLineOptionPossibleValueList(string Name, params CommandLineOptionPossibleValue[] PossibleValues)
    {
        this.Name = Name;
        this.PossibleValues = PossibleValues;
    }
}
