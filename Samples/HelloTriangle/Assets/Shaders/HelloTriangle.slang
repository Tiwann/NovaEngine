module HelloTriangle;

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


ConstantBuffer<float> Time;
Sampler2D Texture;

[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Out;
    const float OffsetX = cos(Time) * 0.25;
    const float OffsetY = sin(Time) * 0.25;

    Out.Position = float4(Input.Position, 1.0);
    //Out.Position.x = Out.Position.x + OffsetX;
    //Out.Position.y = Out.Position.y + OffsetY;
    Out.Position.z = (Out.Position.z + Out.Position.w) * 0.5;
    Out.TexCoord = Input.TexCoord;
    Out.TexCoord.y = 1.0 - Out.TexCoord.y;
    Out.Normal = Input.Normal;
    Out.Color = Input.Color;
    return Out;
}

[shader("fragment")]
float4 FragmentMain(FragmentInput Input) : SV_Target
{
    float4 Sampled = Texture.Sample(Input.TexCoord);
    return Sampled;
    return float4(Input.TexCoord.xy, 0.0, 1.0);
}