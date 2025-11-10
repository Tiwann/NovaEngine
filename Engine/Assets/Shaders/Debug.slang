module Debug;
import Common;

struct VertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
}

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color;
};

[vk::push_constant] ConstantBuffer<float4x4, Std430DataLayout> viewProj;

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
    VertexOutput output;
    output.position = viewProj * float4(input.position, 1.0);
    output.color = input.color;
    return output;
}

[shader("fragment")]
float4 frag(VertexOutput input) : SV_Target
{
	return input.color;
}