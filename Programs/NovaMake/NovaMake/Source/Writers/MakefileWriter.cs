using System.Text;

namespace NovaMake;

public class MakefileWriter(Stream Output, Solution Solution) : IDisposable
{
    public void Write()
    {
        StringBuilder Builder = new StringBuilder();
        Builder.AppendLine("CXX = g++");
        
    }

    public void Dispose()
    {
        Output.Dispose();
    }
    
}