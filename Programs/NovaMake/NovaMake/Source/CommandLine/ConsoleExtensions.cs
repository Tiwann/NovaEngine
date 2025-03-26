namespace NovaMake;

public static class ConsoleExtensions
{
    public static ConsoleKey WaitForKeys(params ConsoleKey[] Keys)
    {
        bool BadKey = true;
        ConsoleKey Input = ConsoleKey.NoName;
        while (BadKey)
        {
            Input = Console.ReadKey().Key;
            BadKey = !Keys.Any(K => K == Input);
        }
        return Input;
    }
}