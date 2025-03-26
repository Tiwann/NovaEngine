using System.Xml;

namespace NovaMake;

public class VcxprojWriter(Stream Output, Project Project) : XmlCustomWriter(Output)
{
    
    private void WriteProperty(string Name, CPPStandard Standard)
    {
        Writer.WriteStartElement(Name);
        Writer.WriteString(GetStandard(Standard));
        Writer.WriteEndElement();
    }
    
    private void WriteProperty(string Name, CStandard Standard)
    {
        Writer.WriteStartElement(Name);
        Writer.WriteString(GetStandard(Standard));
        Writer.WriteEndElement();
    }
    
    private void WriteProperty(string Name, OutputType Type)
    {
        Writer.WriteStartElement(Name);
        Writer.WriteString(GetConfigType(Type));
        Writer.WriteEndElement();
    }
    
    private void WriteConfigurations(Solution Solution)
    {
        WriteElement("ItemGroup", ("Label", "ProjectConfigurations"), delegate
        {
            foreach (Configuration Configuration in Solution.Configurations)
            {
                if (Solution.Platforms.Count > 0)
                {
                    foreach (string Platform in Solution.Platforms)
                    {
                        string ConfigName = $"{Configuration.Name} {Platform}";
                        WriteElement("ProjectConfiguration", ("Include", $"{ConfigName}|{Solution.Architecture}"), delegate
                        {
                            WriteProperty("Configuration", ConfigName);
                            WriteProperty("Platform", $"{Solution.Architecture}");    
                        });
                    }
                }
                else
                {
                    string ConfigName = $"{Configuration.Name}";
                    WriteElement("ProjectConfiguration", ("Include", $"{ConfigName}|{Solution.Architecture}"), delegate
                    {
                        WriteProperty("Configuration", ConfigName);
                        WriteProperty("Platform", $"{Solution.Architecture}");
                    });
                }
            }
        });
    }
    

    private string GetConfigType(OutputType Type)
    {
        return Type switch
        {
            OutputType.Console or OutputType.Windowed => "Application",
            OutputType.SharedLibrary => "SharedLibrary",
            OutputType.StaticLibrary => "StaticLibrary",
            _ => throw new ArgumentOutOfRangeException()
        };
    }

    private string GetStandard(CPPStandard Standard)
    {
        return Standard switch
        {
            CPPStandard.CPP20 => "stdcpp20",
            CPPStandard.CPP17 => "stdcpp17",
            CPPStandard.CPP14 => "stdcpp14",
            CPPStandard.CPP11 => "stdcpp11",
            CPPStandard.CPPLatest => "stdcpplatest",
            CPPStandard.None => "Default",
            CPPStandard.CPP23 => "stdcpp23",
            _ => throw new ArgumentOutOfRangeException()
        };
    }
    
    private string GetStandard(CStandard Standard)
    {
        return Standard switch
        {
            CStandard.C11 => "stdc11",
            CStandard.C17 => "stdc17",
            CStandard.CLatest => "stdclatest",
            CStandard.None => "Default",
            _ => throw new ArgumentOutOfRangeException()
        };
    }

    // TODO: Rewrite this
    private IEnumerable<string> GetFilesFromWildcard(string Wildcard)
    {
        if (Wildcard.Contains("*") && Wildcard.Contains("**"))
        {
            string SearchDirectory = Path.Combine(Project.Location,
                Wildcard.FindAndTrimEnd("**").TrimEnd('\\', '/'));
            if (!Directory.Exists(SearchDirectory)) return null;
            return Directory.EnumerateFiles(SearchDirectory, $"**{Path.GetExtension(Wildcard)}",
                SearchOption.AllDirectories);
        }

        if (Wildcard.Contains("*") && !Wildcard.Contains("**"))
        {
            string SearchDirectory = Path.Combine(Project.Location,
                Wildcard.FindAndTrimEnd("*").TrimEnd('\\', '/'));
            if (!Directory.Exists(SearchDirectory)) return null;
            return Directory.EnumerateFiles(SearchDirectory, $"*{Path.GetExtension(Wildcard)}",
                SearchOption.TopDirectoryOnly);
        }

        return null;
    }

    public override void Write()
    {
        try
        {
            // Document begin
            Writer.WriteStartDocument();
            Writer.WriteComment($"Nova Make {Version.NovaVersion} generated vcxproj");
            Writer.WriteComment("Nova Make (C) Erwann Messoah 2023");
            Writer.WriteComment("\"https://github.com/Tiwann/NovaMake\"");

            WriteElement("Project", new []{ ("DefaultTargets", "Build"), ("xmlns", XmlStatics.XmlNamespace)}, delegate
            {
                WriteConfigurations(Project.Solution);
                
                WriteElement("Import", ("Project", @"$(VCTargetsPath)\Microsoft.Cpp.Default.props"));
                
                // Write globals
            WriteElement("PropertyGroup", ("Label", "Globals"), delegate
            {
                WriteProperty("TargetName", Project.TargetName);
                WriteProperty("ProjectGuid", Project.Guid);
                WriteProperty("ConfigurationType", Project.Type);
                WriteProperty("CharacterSet", "Unicode");
                WriteProperty("PlatformToolset", "v143");
                WriteProperty("WindowsTargetPlatformVersion", "10.0");
                WriteProperty("RootNamespace", Project.Name);
                WriteProperty("IgnoreWarnCompileDuplicatedFilename", true);
                WriteProperty("Keyword", "Win32Proj");
                if(!string.IsNullOrEmpty(Project.BinariesDirectory)) WriteProperty("OutDir", Project.BinariesDirectory.EndsWith(@"\") ? Project.BinariesDirectory : $@"{Project.BinariesDirectory}\");
                if(!string.IsNullOrEmpty(Project.IntermediateDirectory)) WriteProperty("IntDir", Project.IntermediateDirectory.EndsWith(@"\") ? Project.IntermediateDirectory : $@"{Project.IntermediateDirectory}\");
            });
            
            WriteElement("Import", ("Project", @"$(VCTargetsPath)\Microsoft.Cpp.props"));
            
            WriteElement("ItemDefinitionGroup", ("Label", "Globals"), delegate
            {
                WriteElement("ClCompile", delegate
                {
                    WriteProperty("LanguageStandard", Project.CppStandard);
                    WriteProperty("LanguageStandard_C", Project.CStandard);
                    
                    // Enable modules if c++20 or above
                    WriteProperty("EnableModules", Project.Flags.Contains(ProjectFlags.ModuleSupport) && Project.CppStandard >= CPPStandard.CPP20);
            
                    // Enable multiprocessor compile
                    WriteProperty("MultiProcessorCompilation", Project.Flags.Contains(ProjectFlags.MultiProcessorCompile));

                    // Wchar_t
                    WriteProperty("TreatWChar_tAsBuiltInType", !Project.Flags.Contains(ProjectFlags.DisableBuiltInWideChar));
                    
                    // Defines
                    if(!Project.Defines.IsEmpty()) WriteProperty("PreprocessorDefinitions", Project.Defines.GetList(';'));
                    
                    // Includes
                    if(!Project.IncludeDirectories.IsEmpty()) WriteProperty("AdditionalIncludeDirectories", Project.IncludeDirectories.GetList(';'));
                });
            });
            

            
            
            // Project references
            if (!Project.Links.IsEmpty())
            {
                Writer.WriteComment("Project references");
                WriteElement("ItemGroup", delegate
                {
                    List<Project> Projects = Project.Solution.Projects.Where(P => Project.Links.Contains(P.Name)).ToList();
                    if (!Projects.IsEmpty())
                    {
                        foreach (Project Proj in Projects)
                        {
                            WriteElement("ProjectReference", ("Include", Proj.TargetPath), delegate
                            {
                                WriteProperty("ProjectReference", Proj.Guid);
                            });
                        }
                    }
                });
            }
            
            // TODO: Better files including handling
            List<string> Files = new List<string>();
            Files.AddRange(Project.Files.Where(F => File.Exists(@$"{Project.Location}\{F}")));
            Project.Files.ForEach(S =>
            {
                var Found = GetFilesFromWildcard(S);
                if (Found != null) Files.AddRange(Found);
            });
            

            List<string> AdditionalFiles = new List<string>();
            Files.AddRange(Project.AdditionalFiles.Where(F => File.Exists(@$"{Project.Location}\{F}")));
            Project.AdditionalFiles.ForEach(S =>
            {
                var Found = GetFilesFromWildcard(S);
                if (Found != null) AdditionalFiles.AddRange(Found);
            });

            IEnumerable<string> AvailableSourceExtensions = new[] { ".c", ".cpp", ".cxx", ".cc" };
            IEnumerable<string> AvailableHeaderExtensions = new[] { ".h", ".hpp", ".hxx", ".hh", ".inl", ".inc" };
            

            if (Files.Count != 0)
            {
                // Sources
                Writer.WriteComment("Including source files");
                WriteElement("ItemGroup", ("Label", "Sources"), delegate
                {
                    foreach (string Source in Files.Distinct())
                    {
                        if (AvailableSourceExtensions.Any(E => Source.EndsWith(E)))
                        {
                            WriteElement("ClCompile", delegate
                            {
                                WriteAttribute("Include", Project.TargetDirectory.GetRelativePath(Path.Combine(Project.Location, Source)));
                            });
                        }
                    }
                });
                
                // Headers
                Writer.WriteComment("Including header files");
                WriteElement("ItemGroup", ("Label", "Headers"), delegate
                {
                    foreach (string Header in Files.Distinct())
                    {
                        if (AvailableHeaderExtensions.Any(E => Header.EndsWith(E)))
                        {
                            WriteElement("ClInclude", delegate
                            {
                                WriteAttribute("Include", Project.TargetDirectory.GetRelativePath(Path.Combine(Project.Location, Header)));
                            });
                        }
                    }
                });
            }

            if (AdditionalFiles.Count != 0)
            {
                // Additional files
                Writer.WriteComment("Including additional files");
                WriteElement("ItemGroup", ("Label", "AdditionalFiles"), delegate
                {
                    foreach (string AddFile in AdditionalFiles.Distinct())
                    {
                        WriteElement("AdditionalFiles", delegate
                        {
                            WriteAttribute("Include", Path.GetFullPath(AddFile));
                        });
                    }
                });
            }
            
            Project.Solution.Configurations.ForEach(Configuration =>
            {
                Project.Configure(Configuration);
                string Condition = $"'$(Configuration)|$(Platform)' == '{Configuration.Name}|{Project.Solution.Architecture}'";
                (string, string) ConditionAttribute = ("Condition", Condition);
                
                WriteElement("PropertyGroup", ConditionAttribute, delegate
                {
                    WriteProperty("OutDir", Project.BinariesDirectory.EndsWith(@"\") ? Project.BinariesDirectory : $@"{Project.BinariesDirectory}\");
                    WriteProperty("IntDir", Project.IntermediateDirectory.EndsWith(@"\") ? Project.IntermediateDirectory : $@"{Project.IntermediateDirectory}\");
                });
                
                WriteElement("ItemDefinitionGroup", ConditionAttribute, delegate
                {
                    if (!Project.Defines.IsEmpty())
                    {
                        WriteElement("ClCompile", delegate
                        {
                            WriteProperty("PreprocessorDefinitions", Project.Defines);
                        });
                    }
                });
            });
            
            WriteElement("Import", ("Project", @"$(VCTargetsPath)\Microsoft.Cpp.targets"));
            });
        }
        catch (Exception Exception)
        {
            Log.Error($"Error while writing Vcxproj: {Exception.Message}");
        }
    }
}