module HelloTriangle;

struct VertexInput
{
  float3 position;
  float4 color;
}

struct VertexOutput
{
  float4 position : SV_Position;
  float4 color;
}

[vk::push_constant] ConstantBuffer<float4x4> mvp;

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
    VertexOutput out;
    float4 position = mul(mvp, float4(input.position, 1.0));
    out.position = position;
    out.color = input.color;
    return out;
}

[shader("fragment")]
float4 frag(VertexOutput input) : SV_Target
{
    return input.color;
}



