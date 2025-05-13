module HelloTriangle;
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

uniform SceneData Scene;

[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Out;
    Out.Position = ConvertToClipSpace(Scene, Input.Position);
    Out.TexCoord = Input.TexCoord;
    Out.Normal = Input.Normal;
    Out.Color = Input.Color;
    return Out;
}


[shader("fragment")]
vec4 FragmentMain(FragmentInput Input) : SV_Target
{
    vec3 LightColor = uDirectionalLight.color.xyz * uDirectionalLight.intensity;
    vec3 N = normalize(Input.normal);
    vec3 L = normalize(uDirectionalLight.direction);
    float Diffuse = saturate(dot(N, L));
    vec3 DiffuseLight = Diffuse * LightColor;
    vec3 AmbientLight = uAmbientLight.color.xyz * uAmbientLight.intensity;

    vec3 V = normalize(uCameraViewDirection);
    vec3 H = normalize(L + V);
    float Specular = saturate(dot(H, N));
    float Glossiness = 0.5;
    float SpecularExponent = exp2(Glossiness * 11.0) + 2.0;
    Specular = pow(Specular, SpecularExponent);

    vec3 FinalColor = AmbientLight + DiffuseLight + Specular;
    return vec4(FinalColor, 1.0);
}