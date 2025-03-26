namespace NovaMake;


/// <summary>
/// Describes a solution's configurations
/// </summary>
public class Configuration 
{
    public string Name { get; set; }
    public ConfigurationFlags Flags;
    public RuntimeType Runtime { get; set; }

    public Configuration(string Name, RuntimeType Runtime, ConfigurationFlags Flags)
    {
        this.Name = Name;
        this.Flags = Flags;
        this.Runtime = Runtime;
    }
}