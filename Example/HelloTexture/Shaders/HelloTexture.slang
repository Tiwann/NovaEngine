module HelloTexture;

struct VertexInput
{
  float3 position;
  float2 texCoords;
}

struct VertexOutput
{
  float4 position : SV_Position;
  float2 texCoords;
}

typedef VertexOutput FragmentInput;

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
    VertexOutput out;
    out.position = float4(input.position, 1.0);
    out.texCoords = input.texCoords;
    return out;
}

[vk::binding(0, 0)] SamplerState sampler;
[vk::binding(1, 0)] Texture2D texture;

[shader("fragment")]
float4 frag(FragmentInput input) : SV_Target
{
    float4 color = texture.Sample(sampler, input.texCoords);
    return color;
}



