﻿module HelloTriangle;

struct VertexInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORDINATE;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
};

typedef struct VertexOutput
{
    float4 Position : SV_Position;
    float2 TexCoord;
    float3 Normal;
    float4 Color;
} FragmentInput;

[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Out;
    Out.Position = float4(Input.Position, 1.0);
    Out.TexCoord = Input.TexCoord;
    Out.Normal = Input.Normal;
    Out.Color = Input.Color;
    return Out;
}

[shader("fragment")]
float4 FragmentMain(FragmentInput Input) : SV_Target
{
    return Input.Color;
}