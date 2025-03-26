namespace NovaMake;


[AttributeUsage(AttributeTargets.Class)]
public class ArgumentParserOptionsAttribute : Attribute
{
    
}

[ArgumentParserOptions]
public class Options
{
    public static readonly CommandLineOption Help;
    public static readonly CommandLineOption Init;
    public static readonly CommandLineOption Build;
    public static readonly CommandLineOption RootDir;
    public static readonly CommandLineOption Source;
    public static readonly CommandLineOption Clean;
    public static readonly CommandLineOption Verbose;
    public static readonly CommandLineOption Install;

    public static class Targets
    {
        public const string VisualStudio = "vs";
        public const string Makefile = "makefile";
        public const string XCode = "xcode";
        public const string CMake = "cmake";
    }

    static Options()
    {
        Help = new CommandLineOption('h', "help", false, false, "Show help");
        Init = new CommandLineOption('i', "init", false, false, "Initialize a workspace",
            new CommandLineOptionPossibleValueList("name"));
        Build = new CommandLineOption('b', "build", false, false, "Generate using Nova Make build scripts",
            new CommandLineOptionPossibleValueList("targets",
            new CommandLineOptionPossibleValue(Targets.VisualStudio,  "Generate Visual Studio Solution (latest version)"),
            new CommandLineOptionPossibleValue(Targets.Makefile, "Generate Makefiles"),
            new CommandLineOptionPossibleValue(Targets.XCode, "Generate XCode Project"),
            new CommandLineOptionPossibleValue(Targets.CMake, "Generate CMakeLists")));
        RootDir = new CommandLineOption('d', "dir", false, false, "Specify a build script search root directory");
        Source = new CommandLineOption('s', "source", false, true, "Add specific build script to the build queue");
        Clean = new CommandLineOption('c', "clean", false, false, "Clean all generated files");
        Verbose = new CommandLineOption('v', "verbose", false, false, "Log all warnings and information");
        Install = new CommandLineOption('I', "install", false, false, "Install NovaMake");
    }


    
}