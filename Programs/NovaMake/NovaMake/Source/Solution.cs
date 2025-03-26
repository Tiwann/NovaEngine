using System.Collections.Generic;
using System.IO;

namespace NovaMake;

public struct CmdCommand
{
    public string Executable;
    public IEnumerable<string> Arguments;
}
public abstract class Solution
{
    public required string Name { get; set; }
    public string TargetDirectory { get; set; } = Directory.GetCurrentDirectory();
    public string Location { get; set; } = Directory.GetCurrentDirectory();
    public List<Configuration> Configurations { get; set; } = [];
    public List<string> Platforms { get; set; } = [];
    public List<System> Systems { get; set; } = [];
    public Architecture Architecture { get; set; } = Architecture.x64;
    public List<string> ProjectNames { get; set; } = [];
    public List<Project> Projects { get; set; } = [];

    public List<string> PostBuildCommands { get; set; } = [];
    public List<CmdCommand> PreBuildCommands { get; set; } = [];
    
    public string NovaMakeFilePath => Path.Combine(Location, Extensions.NovaFile);

    protected void AddConfigurations(params Configuration[] Configs)
    {
        Configurations.AddRange(Configs);
    }
}