module Debug;

struct VertexInput
{
    float3 position;
    float4 color;
}

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color;
};

[vk::push_constant] float4x4 viewProj;

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
    VertexOutput output;
    output.position = mul(viewProj, float4(input.position, 1.0));
    output.color = input.color;
}

[shader("fragment")]
float4 frag(VertexOutput input)
{
	return input.color;
}