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

typedef VertexOutput FragmentInput;

[vk::push_constant] ConstantBuffer<float4x4> mvp;

float4 rotate(float4 pos, float degrees)
{
    float2x2 rotation = float2x2(cos(radians(degrees)), sin(radians(degrees)), -sin(radians(degrees)), cos(radians(degrees)));
    return float4(mul(rotation, pos.xy), 0.0, 1.0);
}

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
float4 frag(FragmentInput input) : SV_Target
{
    return input.color;
}



