module Font;

import Common;

struct VertexInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR;
}

typedef struct VertexOutput
{
    float4 Position;
    float2 TexCoord;
    float4 Color;
} FragmentInput;


ConstantBuffer<Nova::Scene> Scene;

[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Out;
    Out.Position = Scene.LocalToClipPosition(vec4(Input.Position, 1.0));
    Out.TexCoord = Input.TexCoord;
    Out.Color = Input.Color;
    return Out;
}

[shader("fragment")]
vec4 FragmentMain(FragmentInput Input) : SV_Target
{
    return float4(1.0, 1.0, 1.0, 1.0);
}