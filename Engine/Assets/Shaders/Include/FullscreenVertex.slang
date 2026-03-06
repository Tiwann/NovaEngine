module FullscreenVertex;

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

static const VertexInput input[] =
{
    {float2(-1.0, +1.0), float2(0.0, 1.0)},
    {float2(+1.0, +1.0), float2(1.0, 1.0)},
    {float2(+1.0, -1.0), float2(1.0, 0.0)},
    {float2(-1.0, -1.0), float2(0.0, 0.0)},
};

static const uint indices[] = { 0, 2, 1, 0, 3, 2 };

[shader("vertex")]
VertexOutput vert(uint32_t vertexID : SV_VertexID)
{
    uint32_t index = indices[vertexID];
    VertexOutput output;
    output.position = float4(input[index].position, 0.0, 1.0);
    output.uv = input[index].uv;
    return output;
}