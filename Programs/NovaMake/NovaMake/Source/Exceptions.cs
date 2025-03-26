using System;

namespace NovaMake;

public class InvalidCommandLineArgumentException : Exception
{
    public InvalidCommandLineArgumentException(string message) : base(message)
    {
        
    }
}

public class NoArgumentProvidedException : Exception
{
    public NoArgumentProvidedException(string Message) : base(Message)
    {
        
    }
}

public class ScriptCompilationFailedException : Exception
{
    public ScriptCompilationFailedException(string Message) : base(Message)
    {
        
    }
}

public class BuildFailedException : Exception
{
    public BuildFailedException(string Message) : base(Message)
    {
        
    }
}