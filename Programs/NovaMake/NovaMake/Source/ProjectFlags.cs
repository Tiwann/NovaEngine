using System;

namespace NovaMake;

[Flags]
public enum ProjectFlags
{
    None = 0,
    MultiProcessorCompile = 1 << 0,
    ModuleSupport = 1 << 1,
    Optimize = 1 << 2,
    DebugSymbols = 1 << 3,
    DisableBuiltInWideChar = 1 << 4
}