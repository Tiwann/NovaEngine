module HelloCube;
import Common;

struct VertexInput
{
    vec3 Position : POSITION;
    vec2 TexCoord : TEXCOORDINATE;
    vec3 Normal : NORMAL;
    vec4 Color : COLOR;
};

typedef struct VertexOutput
{
    vec4 Position : SV_Position;
    vec2 TexCoord;
    vec3 Normal;
    vec4 Color;
} FragmentInput;

ConstantBuffer<Nova::Scene> Scene;

[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Out;
    Out.Position = Scene.LocalToClipPosition(vec4(Input.Position, 1.0));
    Out.TexCoord = Input.TexCoord;
    Out.Normal = Input.Normal;
    Out.Color = Input.Color;
    return Out;
}


[shader("fragment")]
vec4 FragmentMain(FragmentInput Input) : SV_Target
{
    vec3 LightColor = Scene.DirLightColor * Scene.DirLightIntensity;
    vec3 N = normalize(Input.Normal);
    vec3 L = normalize(Scene.DirLightDir);
    float Diffuse = saturate(dot(N, L));
    vec3 DiffuseLight = Diffuse * LightColor;
    vec3 AmbientLight = Scene.AmbLightColor * Scene.AmbLightIntensity;

    vec3 V = normalize(Scene.CameraViewDirection);
    vec3 H = normalize(L + V);
    float Specular = saturate(dot(H, N));
    float Glossiness = 1.0;
    float SpecularExponent = exp2(Glossiness * 11.0) + 2.0;
    Specular = pow(Specular, SpecularExponent);

    vec3 FinalColor = AmbientLight + DiffuseLight + Specular;
    return vec4(FinalColor, 1.0);
}