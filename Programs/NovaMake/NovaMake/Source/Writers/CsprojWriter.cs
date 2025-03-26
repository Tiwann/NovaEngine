namespace NovaMake;

public class CsprojWriter(Stream Output, Project Project) : XmlCustomWriter(Output)
{
    public override void Write()
    {
        try
        {
            WriteCsproj();
        }
        catch (Exception Exception)
        {
            Log.Error($"Failed to write csproj file: {Exception.Message}");
            Environment.Exit(-1);
        }
    }

    private string GetTargetFramework(DotNetSDK Sdk)
    {
        return Sdk switch
        {
            DotNetSDK.DotNet5 => "net5.0",
            DotNetSDK.DotNet6 => "net6.0",
            DotNetSDK.DotNet7 => "net7.0",
            DotNetSDK.DotNet8 => "net8.0",
            DotNetSDK.DotNetFramework => "netframework4.8.1",
            _ => throw new ArgumentOutOfRangeException(nameof(Sdk), Sdk, null)
        };
    }
    
    private void WriteCsproj()
    {
        WriteElement("Project", ("Sdk", "Microsoft.NET.Sdk"), delegate
        {
            WriteElement("PropertyGroup", delegate
            {

                string Output = Project.Type < OutputType.SharedLibrary ? "Exe" : "Dll";
                WriteProperty("OutputType", Output);
                WriteProperty("TargetFramework", GetTargetFramework(Project.DotNetSdk));
            });
            
            /*<ItemGroup>
                <Compile Include="..\..\Corridor-GareDeLyon-M14\Assets\Gare De Lyon\Scripts\Annonces.cs">
                <Link>Annonces.cs</Link>
                </Compile>
                </ItemGroup>*/
            
            WriteElement("ItemGroup", delegate
            {
                Project.AdditionalFiles.ForEach(F =>
                {
                    WriteElement("Compile", ("Include", F), delegate
                    {
                        WriteProperty("Link", Path.GetFileName(F));
                    });
                });
            });
        });
    }
}