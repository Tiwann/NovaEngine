using System;

namespace NovaMake;

[Flags]
public enum System
{
    None = 0,
    Windows,
    Linux,
    MacOS,
    Android,
    IOS,
    XboxSeries,
    PS5,
    Switch
}