using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.Emit;

namespace NovaMake;

public enum BuildTaskType
{
    VisualStudioSolution,
    Makefiles,
    CMake,
    XCodeProject,
}

public class BuildTask
{
    private readonly BuildTaskType BuildType;
    public List<string> BuildScripts { get; } = [];
    public string RootDirectory { get; set; } = Directory.GetCurrentDirectory();
    
    private EmitResult CompilerResults;
    private Assembly CompiledAssembly;

    public BuildTask(string BuildType, IEnumerable<string> PredefinedSource)
    {
        this.BuildType = BuildType switch
        {
            Options.Targets.VisualStudio => BuildTaskType.VisualStudioSolution,
            Options.Targets.Makefile => BuildTaskType.Makefiles,
            Options.Targets.XCode => BuildTaskType.XCodeProject,
            Options.Targets.CMake => BuildTaskType.CMake,
            _ => throw new BuildFailedException($"Failed to build solution: No such a valid target ({BuildType}).")
        };

        if(PredefinedSource is null) 
            SearchScripts(); 
        else 
            AddScripts(PredefinedSource);
    }

    private void SearchScripts()
    {
        Log.Trace("> Searching for build scripts...");
        List<string> FoundBuildScripts = Directory.EnumerateFiles(RootDirectory, "*.Nova.cs", SearchOption.AllDirectories).ToList();
        FoundBuildScripts.AddRange(Directory.EnumerateFiles(RootDirectory, "*.Nova.csx", SearchOption.AllDirectories));
        
        if (FoundBuildScripts.IsEmpty())
        {
            throw new ScriptCompilationFailedException("No build scripts found!");
        }
        
        BuildScripts.AddRange(FoundBuildScripts);
        Log.Trace($"Found {FoundBuildScripts.Count} build script(s):");
        FoundBuildScripts.ForEach(B => Log.Trace($"--> {B}"));
    }

    public void AddScript(string Script)
    {
        BuildScripts.Add(Script);
    }

    public void AddScripts(IEnumerable<string> Scripts)
    {
        BuildScripts.AddRange(Scripts);
    }

    public void AddScripts(params string[] Scripts)
    {
        BuildScripts.AddRange(Scripts);
    }

    private List<MetadataReference> GetReferences()
    {
        List<string> ReferenceNames = [
            "System.dll",
            "System.Private.CoreLib.dll",
            "System.Runtime.dll",
            "System.Console.dll",
            "netstandard.dll",
            "System.Text.RegularExpressions.dll",
            "System.Linq.dll",
            "System.Linq.Expressions.dll",
            "System.IO.dll",
            "System.Reflection.dll",
            "System.Collections.dll",
            "System.Collections.Concurrent.dll",
            "System.Collections.NonGeneric.dll",
            "System.Collections.Specialized.dll",
            "Microsoft.CSharp.dll"
        ];
        
        string ReferencesPath = Path.GetDirectoryName(typeof(object).Assembly.Location)!;
        List<MetadataReference> References = ReferenceNames.Select(Ref => MetadataReference.CreateFromFile(Path.Combine(ReferencesPath, Ref)))
            .Cast<MetadataReference>().ToList();
        References.Add(MetadataReference.CreateFromFile(Assembly.GetExecutingAssembly().Location));
        return References;
    }

    public void Compile()
    {
        if (BuildScripts.Count <= 0)
        {
            throw new ScriptCompilationFailedException("No script found to compile.");
        }
        
        foreach (var BuildScript in BuildScripts.Where(BuildScript => !File.Exists(BuildScript)))
        {
            throw new FileNotFoundException($"File {BuildScript} was not found/doesn't exists!");
        }
        
        Log.Trace("> Compiling scripts...");
        Stopwatch Stopwatch = new();
        Stopwatch.Start();

        
        List<string> Sources = BuildScripts.Select(File.ReadAllText).ToList();
        List<SyntaxTree> SyntaxTrees = [];
        Sources.ForEach(Source => SyntaxTrees.Add(SyntaxFactory.ParseSyntaxTree(Source)));

        CSharpCompilationOptions CompilerOptions = new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary,
            optimizationLevel: OptimizationLevel.Release);

        CSharpCompilation Compiler =
            CSharpCompilation.Create("NovaMakeRuntimeCompiler", SyntaxTrees, GetReferences(), CompilerOptions);
        

        using Stream Output = new MemoryStream();
        CompilerResults = Compiler.Emit(Output);

        if (!CompilerResults.Success)
        {
            StringBuilder Builder = new();
            foreach (Diagnostic CompileError in CompilerResults.Diagnostics)
            {
                Builder.AppendLine($"{CompileError}.");
            }
            throw new ScriptCompilationFailedException(Builder.ToString());
        }

        CompiledAssembly = Assembly.Load(((MemoryStream)Output).ToArray());
        Stopwatch.Stop();
        Log.Success($"> Compilation successful! Took {Stopwatch.ElapsedMilliseconds / 1000.0f}s.");
    }

    public void Build()
    {
        if (CompiledAssembly is null) throw new BuildFailedException("Compiled Assembly is null!");

        Type SolutionType;
        try
        {
            SolutionType = CompiledAssembly.GetTypes().Single(Type =>
                Type.IsSubclassOf<Solution>() &&
                Type.GetCustomAttribute<BuildAttribute>() is not null);
        }
        catch (Exception)
        {
            throw new BuildFailedException("Build failed because no Solution was declared in the build scripts");
        }
        
        
        Solution Solution = SolutionType.CreateInstance<Solution>()!;
        if (Solution is null)
        {
            throw new BuildFailedException("Failed to instantiate Solution!");
        }

        if (string.IsNullOrEmpty(Solution.Name))
        {
            throw new BuildFailedException($"Solution \"{Solution.GetType().Name}\" needs a name.");
        }
        
        if (Solution.Name.Contains(' '))
        {
            string NewName = Solution.Name.Replace(' ', '_');
            Log.Warning($"> Solution \"{Solution.Name}\" was renamed to \"{NewName}\".");
            Solution.Name = NewName;
        }
        
        Solution.Configurations.ForEach(Conf =>
        {
            if (Conf.Name.Contains(' '))
            {
                string NewName = Conf.Name.Replace(' ', '_');
                Log.Warning($"Project \"{Solution.Name}\" was renamed to \"{NewName}\".");
                Conf.Name = NewName;
            }
        });
        
        if (string.IsNullOrEmpty(Solution.TargetDirectory))
        {
            throw new BuildFailedException($"Please specify a target directory for solution \"{Solution.Name}\"");
        }
        
        if (Solution.Configurations.IsEmpty())
        {
            throw new BuildFailedException($"Solution \"{Solution.GetType().Name}\" needs a set of configurations.");
        }

        if (!Solution.Systems.IsEmpty())
        {
            var NewPlatforms = new List<string>();
            Solution.Systems.ForEach(Sys =>
            {
                if (!Solution.Platforms.IsEmpty())
                {
                    Solution.Platforms.ForEach(Platform =>
                    {
                        NewPlatforms.Add($"{Platform}{Sys.ToString()}");
                    });
                }
                else
                {
                    NewPlatforms.Add(Sys.ToString());
                }
            });
            Solution.Platforms = NewPlatforms;
        }
        
        
        List<Type> ProjectTypes = CompiledAssembly.GetTypes().Where(Type =>
            Type.IsSubclassOf<Project>() &&
            Type.GetCustomAttribute<BuildAttribute>() != null).ToList();

        if (ProjectTypes.IsEmpty()) throw new BuildFailedException("No Projects found!");
        
        foreach (var ProjectInstance in ProjectTypes.Select(Type => Type.CreateInstance<Project>(Solution))
                     .Where(ProjectInstance => Solution.ProjectNames.Contains(ProjectInstance.Name)))
        {
            Solution.Projects.Add(ProjectInstance);
        }
        
        Solution.Projects.ForEach(Project =>
        {
            if (string.IsNullOrEmpty(Project.Name))
            {
                throw new BuildFailedException($"Project \"{Project.GetType().Name}\" needs a name.");
            }

            if (Project.Name.Contains(' '))
            {
                string NewName = Project.Name.Replace(' ', '_');
                Project.Name = NewName;
                Log.Warning($"Project \"{Project.Name}\" was renamed to \"{NewName}\".");
            }
            
            if (string.IsNullOrEmpty(Project.TargetDirectory))
            {
                throw new BuildFailedException($"Please specify a target directory for project \"{Project.Name}\".");
            }
            
            if (string.IsNullOrEmpty(Project.Location))
            {
                throw new BuildFailedException($"Please specify a location for project \"{Project.Name}\".");
            }
            
            foreach (var Filter in Project.Filters)
            {
                switch (Filter.KeyValue.Item1)
                {
                    case "Configuration":
                        IEnumerable<string> Names = Solution.Configurations.Select(C => C.Name);
                        if (Names.Contains(Filter.KeyValue.Item2))
                            Filter.Action.Invoke();
                        break;
                    case "System":
                        IEnumerable<string> Systems = Solution.Systems.Select(S => nameof(S));
                        if(Systems.Contains(Filter.KeyValue.Item2))
                            Filter.Action.Invoke();
                        break;
                    case "Architecture":
                        if($"{Solution.Architecture}" == Filter.KeyValue.Item2)
                            Filter.Action.Invoke();
                        break;
                }
            }
        });
        
        foreach (var Command in Solution.PreBuildCommands)
        {
            ProcessStartInfo Info = new ProcessStartInfo();
            Info.CreateNoWindow = false;
            Info.UseShellExecute = false;
            Info.FileName = Command.Executable;

            StringBuilder Builder = new StringBuilder();
            foreach (var Arg in Command.Arguments)
            {
                Builder.Append($"{Arg} ");
            }

            Info.Arguments = Builder.ToString();
            Info.RedirectStandardOutput = true;
            Info.RedirectStandardError = true;

            Process Proc = new Process();
            Proc.StartInfo = Info;
            Log.Trace($"> Executing command: {Command.Executable} {Builder}");
            if (!Proc.Start())
            {
                Log.Error($"Failed to execute command: {Command}");
                Environment.Exit(-1);
            }
        }

        switch (BuildType)
        {
            case BuildTaskType.VisualStudioSolution:
                BuildVisualStudioSolution(Solution);
                break;
            case BuildTaskType.Makefiles:
                BuildMakefiles(Solution);
                break;
            case BuildTaskType.CMake:
                BuildCMake(Solution);
                break;
            case BuildTaskType.XCodeProject:
                BuildXCodeProject(Solution);
                break;
            default:
                throw new ArgumentOutOfRangeException();
        }
        
    }
    
    private void BuildVisualStudioSolution(Solution Solution)
    {
        Log.Trace("> Generating Visual Studio Solution...");
        Stopwatch Stopwatch = new();
        Stopwatch.Start();
        
        List<string> GeneratedFiles = [];
        
        // Write vcxproj files
        foreach (Project Project in Solution.Projects)
        {
            if (!Directory.Exists(Project.TargetDirectory))
            {
                Directory.CreateDirectory(Project.TargetDirectory);
            }
            
            string Filepath = Path.ChangeExtension(Project.TargetPath, Extensions.VisualStudio.CXXProject)!;
            GeneratedFiles.Add(Filepath);
            using FileStream Stream = File.Open(Filepath, FileMode.OpenOrCreate, FileAccess.Write);
            Stream.SetLength(0);
            
            using VcxprojWriter Writer = new(Stream, Project);
            Writer.Write();
            Log.Trace($"> Generated {Filepath}");
        }
        
        #if ASTRO_BUILD_CSPROJ
        //Write astro.csproj file
        Project NovaProject = new ConsoleApplication(null)
        {
            Name = "NovaMake",
            TargetName = "NovaMake",
            TargetDirectory = Solution.Location,
            BinariesDirectory = Path.Combine(Solution.Location, "bin"),
            IntermediateDirectory = Path.Combine(Solution.Location, "obj"),
            Language = Language.CSharp,
            DotNetSdk = DotNetSDK.DotNet8,
            Location = Solution.Location,
        };
        NovaProject.AdditionalFiles.AddRange(BuildScripts);
        Solution.Projects.Add(NovaProject);
        
        string CsprojFilepath = Path.ChangeExtension(Path.Combine(NovaProject.Location, NovaProject.TargetName),
            Extensions.VisualStudio.CSharpProject);
        using FileStream CsprojStream = new FileStream(CsprojFilepath, FileMode.OpenOrCreate, FileAccess.Write);
        CsprojStream.SetLength(0);
        
        
        using CsprojWriter CsprojWriter = new CsprojWriter(CsprojStream, NovaProject);
        CsprojWriter.Write();
        GeneratedFiles.Add(CsprojFilepath);
        Log.Trace($"> Generated {CsprojFilepath}");
        #endif

        // Write Sln file
        if (!Directory.Exists(Solution.TargetDirectory))
            Directory.CreateDirectory(Solution.TargetDirectory);
        
        string SlnFilepath = $"{Solution.TargetDirectory}\\{Solution.Name}{Extensions.VisualStudio.Solution}";
        GeneratedFiles.Add(SlnFilepath);
        using FileStream SlnStream = new (SlnFilepath, FileMode.OpenOrCreate, FileAccess.Write);
        using SlnWriter SlnWriter = new (this, SlnStream, Solution);
        SlnStream.SetLength(0);
        SlnWriter.Write();
        Log.Trace($"> Generated {SlnFilepath}");

        Log.Trace($"> Writing {Solution.NovaMakeFilePath}...");
        CreateNovaMakeFile(GeneratedFiles, Solution);
        Stopwatch.Stop();
        Log.Success($"> Visual Studio Solution generation successful! Took {Stopwatch.ElapsedMilliseconds / 1000.0f}s");
    }

    private void BuildMakefiles(Solution Solution)
    {
        throw new NotImplementedException("This feature is not yet implemented.");
        string Filepath = $"{Solution.TargetDirectory}\\{Extensions.Linux.Makefile}";
        using FileStream Stream = new FileStream(Filepath, FileMode.CreateNew, FileAccess.Write);
        using MakefileWriter Writer = new MakefileWriter(Stream, Solution);
        Stream.SetLength(0);
        Writer.Write();
    }
    
    private void BuildCMake(Solution Solution)
    {
        throw new NotImplementedException();
    }
    
    private void BuildXCodeProject(Solution Solution)
    {
        throw new NotImplementedException();
    }

    private void CreateNovaMakeFile(IEnumerable<string> GeneratedFiles, Solution Solution)
    {
        string Filepath = Solution.NovaMakeFilePath;
        StringBuilder Builder = new();
        Builder.AppendLine($"# Nova Make {Version.NovaVersion}");
        Builder.AppendLine("# (C) Erwann Messoah 2023");
        Builder.AppendLine("# https://github.com/Tiwann/NovaMake\n");
        
        foreach (string GeneratedFile in GeneratedFiles)
        {
            Builder.AppendLine(GeneratedFile);
        }

        if (Solution.TargetDirectory != RootDirectory)
        {
            Builder.AppendLine(Solution.TargetDirectory);
        }
        File.WriteAllText(Filepath, Builder.ToString());
    }
}