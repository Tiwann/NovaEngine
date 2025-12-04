module Fullscreen;

struct VertexInput
{
    float2 position : POSITION;
    float2 uv : TEXCOORD;
}

struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv;
};

[[vk::binding(0, 0)]] Sampler2D texture;

static const VertexInput input[] =
{
    {float2(-1.0, +1.0), float2(0.0, 1.0)},
    {float2(+1.0, +1.0), float2(1.0, 1.0)},
    {float2(+1.0, -1.0), float2(1.0, 0.0)},
    {float2(-1.0, -1.0), float2(0.0, 0.0)},
};

static const uint indices[] = { 0, 2, 1, 0, 3, 2 };

[shader("vertex")]
VertexOutput vert(uint vertexID : SV_VertexID)
{
    uint index = indices[vertexID];
    VertexOutput output;
    output.position = float4(input[index].position, 0.0, 1.0);
    output.uv = input[index].uv;
    return output;
}

float3 ACESFilm(float3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

[shader("fragment")]
float4 frag(VertexOutput input) : SV_Target
{
    float4 color = texture.Sample(input.uv);
    return color;
}