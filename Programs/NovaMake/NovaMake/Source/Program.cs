using System.Diagnostics;
using System.Runtime.InteropServices;

namespace NovaMake;

internal static class Program
{
    private static System GetCurrentSystem()
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows)) return System.Windows;
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux)) return System.Linux;
        if (RuntimeInformation.IsOSPlatform(OSPlatform.OSX)) return System.MacOS;
        throw new Exception("This operating system is not supported yet.");
    }

    private static Architecture GetCurrentArchitecture()
    {
        return RuntimeInformation.OSArchitecture switch
        {
            global::System.Runtime.InteropServices.Architecture.X64 => Architecture.x64,
            global::System.Runtime.InteropServices.Architecture.Arm64 => Architecture.ARM64,
            global::System.Runtime.InteropServices.Architecture.X86 => Architecture.x86,
            _ => throw new Exception("This architecture is not yet supported.")
        };
    }

    private static void Clean()
    {
        Log.Trace("> Cleaning all generated files...");
        if (!File.Exists(Extensions.NovaFile))
        {
            Log.Error($"{Extensions.NovaFile} file not found!");
            Environment.Exit(-1);
        }

        List<string> FileContent = File.ReadLines(Extensions.NovaFile).ToList();
        IEnumerable<string> Files = FileContent.Where(S => !S.StartsWith("#") && File.Exists(S));
        IEnumerable<string> Directories = FileContent.Where(S => !S.StartsWith("#") && Directory.Exists(S));
        foreach (string F in Files)
        {
            Log.Trace($"> Deleting {F}");
            File.Delete(F);
        }
        
        foreach (string D in Directories)
        {
            Log.Trace($"> Deleting {D}");
            Directory.Delete(D);
        }
        
        File.Delete(Extensions.NovaFile);
        Log.Success("> Done!");
        Environment.Exit(0);
    }
    
    private static int Main(string[] Arguments)
    {
        // Hello Nova Make
        Log.Trace($"Nova Make {Version.NovaVersion}");
        Log.Trace("Copyright (C) 2025 Erwann Messoah");

        System CurrentSystem = System.None;
        Architecture CurrentArchitecture = Architecture.None;
        try
        {
            CurrentSystem = GetCurrentSystem();
            CurrentArchitecture = GetCurrentArchitecture();
        }
        catch (Exception Exception)
        {
            Log.Error(Exception.Message);
            Environment.Exit(-1);
        }
        
        
        // Setting up the parser
        ArgumentParserSettings Settings = CurrentSystem switch
        {
            System.Windows => ArgumentParserSettings.WindowsStyle,
            System.Linux => ArgumentParserSettings.LinuxStyle,
            System.MacOS => ArgumentParserSettings.LinuxStyle,
        };
        ArgumentParser Parser = new(Arguments, Settings);
        
        // Parse the arguments
        try
        {
            Parser.AddOptions<Options>();
            Parser.Parse();
        }
        catch (NullReferenceException Exception)
        {
            Log.Error(Exception.Message);
            Environment.Exit(-1);
        }
        catch (InvalidCommandLineArgumentException Exception)
        {
            Log.Error(Exception.Message);
            Log.Trace(Parser.GetHelpText());
            Environment.Exit(-1);
        }
        catch (ArgumentException Exception)
        {
            Log.Error(Exception.Message);
            Environment.Exit(-1);
        }
        catch (NoArgumentProvidedException Exception)
        {
            Log.Error(Exception.Message);
            Environment.Exit(-1);
        }
        
        // Handle help
        if (Parser.GetValue<bool>(Options.Help))
        {
            Log.Trace(Parser.GetHelpText());
            Environment.Exit(0);
        }
        
        
        // Handle build
        if (Parser.GetBool(Options.Build))
        {
            try
            {
                // Create build task
                IEnumerable<string> PredefinedSources = Parser.GetValues(Options.Source);
                BuildTask Task = new BuildTask(Parser.GetValue<string>(Options.Build), PredefinedSources);
                if (Parser.GetValue<bool>(Options.RootDir))
                {
                    Task.RootDirectory = Parser.GetValue<string>(Options.RootDir);
                }

                Task.Compile();
                Task.Build();
                Environment.Exit(0);
            }
            catch (ScriptCompilationFailedException Exception)
            {
                Log.Error(Exception.Message);
            }
            catch (BuildFailedException Exception)
            {
                Log.Error(Exception.Message);
                Environment.Exit(-1);
            }
            catch (DirectoryNotFoundException Exception)
            {
                Log.Error(Exception.Message);
            }
            catch (NotImplementedException)
            {
                Log.Error("This function is not yet available. Please wait for future updates.");
                Environment.Exit(0);
            }
        }

        return 0;
    }
}

