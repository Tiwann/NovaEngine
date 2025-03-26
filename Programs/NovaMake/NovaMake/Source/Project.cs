using System;
using System.Collections.Generic;
using System.IO;

namespace NovaMake;

public struct Filter
{
    public (string, string) KeyValue;
    public Action Action;
}

/// <summary>
/// Describes an Application/Project
/// </summary>
public abstract class Project
{
    public Solution Solution { get; set; }
    public required string Name { get; set; }
    public string TargetDirectory { get; set; }
    public string TargetName { get; set; }
    public required string Location { get;  set; }
    public required string BinariesDirectory { get; set; }
    public required string IntermediateDirectory { get; set; }

    private string Extension
    {
        get
        {
            return Language switch
            {
                Language.C or Language.CPlusPlus => Extensions.VisualStudio.CXXProject,
                Language.CSharp => Extensions.VisualStudio.CSharpProject,
                _ => throw new ArgumentOutOfRangeException()
            };
        }
    }
    
    public string TargetPath => Path.ChangeExtension(Path.Combine(TargetDirectory, TargetName), Extension);
    public OutputType Type { get; protected init; }
    public Language Language { get; set; }
    public ProjectFlags Flags { get; protected set; }
    public CPPStandard CppStandard { get; set; } = CPPStandard.CPP20;
    public CStandard CStandard { get; set; } = CStandard.C17;
    public CSharpVersion CSharpVersion { get; set; } = CSharpVersion.CSharp11;
    public DotNetSDK DotNetSdk { get; set; } = DotNetSDK.DotNet8;
    public List<string> Files { get; protected set; }
    public List<string> AdditionalFiles { get; protected set; }
    public List<string> IncludeDirectories { get; protected set; }
    public List<string> LibrariesDirectories { get; protected set; }
    public List<string> Defines { get; protected set; }
    public List<string> Links { get; protected set; }
    public Guid Guid { get; }
    public List<Filter> Filters = new();

    public Guid ProjectTypeGuid
    {
        get
        {
            return Language switch
            {
                Language.C or Language.CPlusPlus => new Guid("8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942"),
                Language.CSharp => new Guid("FAE04EC0-301F-11D3-BF4B-00C04F79EFBC"),
                _ => throw new ArgumentOutOfRangeException()
            };
        }
    }

    protected Project(Solution Solution)
    {
        Files = new();
        AdditionalFiles = new();
        IncludeDirectories = new();
        LibrariesDirectories = new();
        Defines = new();
        Links = new();
        Flags = ProjectFlags.None;
        this.Solution = Solution;
        Guid = Guid.NewGuid();
    }
    
    public virtual void Configure(Configuration Configuration)
    {
        
    }
    
}