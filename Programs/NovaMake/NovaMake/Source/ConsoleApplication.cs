namespace NovaMake;

public class ConsoleApplication : Project
{
    public ConsoleApplication(Solution Solution) : base(Solution) => Type = OutputType.Console;
}