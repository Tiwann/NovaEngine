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

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
    VertexOutput out;
    out.position = float4(input.position, 1.0);
    out.color = input.color;
    return out;
}

[shader("fragment")]
float4 frag(FragmentInput input) : SV_Target
{
    return input.color;
}



