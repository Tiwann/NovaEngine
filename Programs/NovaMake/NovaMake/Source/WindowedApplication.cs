namespace NovaMake;

public class WindowedApplication : Project
{
    public WindowedApplication(Solution Solution) : base(Solution) => Type = OutputType.Windowed;
}