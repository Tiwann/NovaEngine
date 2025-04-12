module Sprite;

struct VertexInput
{
    float3 Position;
    float2 TexCoord;
    float3 Normal;
    float4 Color;
}

typedef struct VertexOutput
{
    float4 Position;
    float2 TexCoord;
    float3 Normal;
    float4 Color;
} FragmentInput;

struct Camera
{
    float4x4 Model;
    float4x4 View;
    float4x4 Projection;
}

struct Sprite
{
    float3x3 Scale;
    float2 Tiling;
    Texture2D Texture;
    float4 ColorTint;
}

ConstantBuffer<Camera> InCamera;
ConstantBuffer<Sprite> InSprite;
SamplerState Sampler;

[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    float4x4 Mvp = InCamera.Projection * InCamera.View * InCamera.Model;
    float4 ScaledPosition = float4(mul(InSprite.Scale, Input.Position), 1.0);

    VertexOutput Out;
    Out.Position = mul(Mvp, ScaledPosition);
    Out.TexCoord = mul(InSprite.Tiling, Input.TexCoord);
    Out.Normal = Input.Normal;
    Out.Color = Input.Color;
    return Out;
}

[shader("fragment")]
float4 FragmentMain(FragmentInput Input)
{
    float4 Color = InSprite.Texture.Sample(Sampler, Input.TexCoord);
    if(Color.w <= 0.0001)
        discard;

    return Input.Color * InSprite.ColorTint;
}