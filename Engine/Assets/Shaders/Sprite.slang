module Sprite;

struct VertexInput
{
    float3 position;
    float2 texCoords;
    float4 color;
    uint textureIndex;
}

struct VertexOutput
{
    float4 position : SV_Position;
    float2 texCoords;
    float4 color;
    uint textureIndex;
}

typedef VertexOutput FragmentInput;

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
    VertexOutput out;
    out.position = float4(input.position, 1.0);
    out.texCoords = input.texCoords;
    out.color = input.color;
    out.textureIndex = input.textureIndex;
    return out;
}

[vk::binding(0, 0)] Sampler2D samplers[];

[shader("fragment")]
float4 frag(FragmentInput input) : SV_Target
{
    Sampler2D sampler = samplers[input.textureIndex];
    float4 color = sampler.Sample(input.texCoords);
    if(color.a <= 0.001) discard;
    return color;
}



