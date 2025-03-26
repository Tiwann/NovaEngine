using System;

namespace NovaMake;

public static class Log
{
    public static bool Verbose = false;
    
    public static void Trace(string Message)
    {
        Console.ForegroundColor = ConsoleColor.White;
        Console.WriteLine($"{Message}");
        Console.ResetColor();
    }

    public static void Success(string Message)
    {
        Console.ForegroundColor = ConsoleColor.Green;
        Console.WriteLine($"{Message}");
        Console.ResetColor();
    }
    
    public static void Warning(string Message)
    {
        Console.ForegroundColor = ConsoleColor.Yellow;
        Console.WriteLine($"{Message}");
        Console.ResetColor();
    }
    
    public static void Error(string Message)
    {
        Console.ForegroundColor = ConsoleColor.Red;
        Console.WriteLine($"{Message}");
        Console.ResetColor();
    }
    
    public static void TraceVerbose(string Message)
    {
        if (Verbose)
        {
            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine($"{Message}");
            Console.ResetColor();
        }
    }

    public static void SuccessVerbose(string Message)
    {
        if (Verbose)
        {
            Console.ForegroundColor = ConsoleColor.Green;
            Console.WriteLine($"{Message}");
            Console.ResetColor();
        }
    }
    
    public static void WarningVerbose(string Message)
    {
        if (Verbose)
        {
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine($"{Message}");
            Console.ResetColor();  
        }
    }
    
    public static void ErrorVerbose(string Message)
    {
        if (Verbose)
        {
            Console.ForegroundColor = ConsoleColor.Red;
            Console.WriteLine($"{Message}");
            Console.ResetColor();
        }
    }

    public static void ClearLine()
    {
        int currentLineCursor = Console.CursorTop;
        Console.SetCursorPosition(0, currentLineCursor);
        Console.Write(new string(' ', Console.WindowWidth));
        Console.SetCursorPosition(0, currentLineCursor);
    }
}