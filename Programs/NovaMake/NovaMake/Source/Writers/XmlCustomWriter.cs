using System.Xml;

namespace NovaMake;

public class XmlCustomWriter(Stream Output) : IDisposable
{
    protected readonly XmlTextWriter Writer = XmlStatics.CreateWriter(Output);

    public void WriteStartElement(string Name, params (string, string)[] Attributes)
    {
        Writer.WriteStartElement(Name);
        foreach ((string, string) Attribute in Attributes)
        {
            WriteAttribute(Attribute.Item1, Attribute.Item2);
        }
    }
    
    public void WriteElement(string Name, (string, string) Attribute, params Action[] Content)
    {
        Writer.WriteStartElement(Name);
        WriteAttribute(Attribute.Item1, Attribute.Item2);
        foreach (Action Action in Content)
        {
            Action.Invoke();
        }
        Writer.WriteEndElement();
    }

    public void WriteElement(string Name, (string, string)[] Attributes, params Action[] Content)
    {
        Writer.WriteStartElement(Name);
        foreach ((string, string) Attribute in Attributes)
        {
            WriteAttribute(Attribute.Item1, Attribute.Item2);
        }
        foreach (Action Action in Content)
        {
            Action.Invoke();
        }
        Writer.WriteEndElement();
    }
    
    public void WriteElement(string Name, params Action[] Content)
    {
        Writer.WriteStartElement(Name);
        foreach (Action Action in Content)
        {
            Action.Invoke();
        }
        Writer.WriteEndElement();
    }

    public void WriteAttribute(string Name, string Value)
    {
        Writer.WriteStartAttribute(Name);
        Writer.WriteValue(Value);
        Writer.WriteEndAttribute();
    }

    public void WriteProperty(string Name, string Value)
    {
        Writer.WriteStartElement(Name);
        Writer.WriteString(Value);
        Writer.WriteEndElement();
    }
    
    public void WriteProperty(string Name, bool Value)
    {
        Writer.WriteStartElement(Name);
        Writer.WriteString(Value.ToString());
        Writer.WriteEndElement();
    }
    
    public void WriteProperty(string Name, Guid Value)
    {
        Writer.WriteStartElement(Name);
        Writer.WriteString($"{{{Value}}}".ToUpper());
        Writer.WriteEndElement();
        
    }
    
    public void WriteProperty(string Name, IEnumerable<string> List)
    {
        WriteProperty(Name, List.GetList(';'));
    }

    public virtual void Write()
    {
        
    }

    public void Dispose()
    {
        Writer?.Dispose();
    }
}