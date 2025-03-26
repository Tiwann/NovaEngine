public static class Extensions
{
    public const string NovaBuildScript = ".Nova.cs";
    public const string NovaFile = "astromake";

    public static class VisualStudio
    {
        public const string CXXProject = ".vcxproj";
        public const string Solution = ".sln";
        public const string CSharpProject = ".csproj";
        public const string CSharpFile = ".cs";
        public const string CSharpScript = ".csx";
    }

    public static class Linux
    {
        public const string Makefile = "Makefile";
        public const string SharedLib = ".so";
        public const string StaticLib = ".a";
    }

    public static class Mac
    {
        public const string XCodeProject = ".xcodeproj";
        public const string SharedLib = ".dylib";
    }

    public static class Windows
    {
        public const string Executable = ".exe";
        public const string SharedLib = ".dll";
        public const string StaticLib = ".lib";
    }
}