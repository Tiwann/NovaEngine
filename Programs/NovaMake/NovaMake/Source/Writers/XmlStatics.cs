using System.Text;
using System.Xml;

namespace NovaMake;

public static class XmlStatics
{
    public static readonly string XmlNamespace = "http://schemas.microsoft.com/developer/msbuild/2003";

    public static XmlTextWriter CreateWriter(Stream Output)
    {
        XmlTextWriter writer = new (Output, Encoding.UTF8);
        writer.Formatting = Formatting.Indented;
        writer.Indentation = 4;
        writer.Namespaces = true;
        return writer;
    }
}

