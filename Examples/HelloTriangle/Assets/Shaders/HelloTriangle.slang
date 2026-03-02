module HelloTriangle;

struct Vertex
{
	float2 position;
	float4 color;
};

public struct VertexOutput
{
    public float4 position : SV_Position;
    public float4 color;
};

static const Vertex vertices[3] =
{
	{float2(-0.5, -0.5), float4(1.0, 0.0, 0.0, 1.0)},
	{float2(+0.0, +0.5), float4(0.0, 1.0, 0.0, 1.0)},
	{float2(+0.5, -0.5), float4(0.0, 0.0, 1.0, 1.0)},
};

static const uint32_t indices[3] = {0, 2, 1};

[shader("vertex")]
VertexOutput vert(uint32_t vertexID : SV_VertexID)
{
	uint32_t index = indices[vertexID];
	Vertex vertex = vertices[index];

	float2 position = vertex.position;

	VertexOutput output;
	output.position = float4(position, 0.0, 1.0);
	output.color = vertex.color;
	return output;
}

[shader("fragment")]
float4 frag(VertexOutput input) : SV_Target
{
	return input.color;
}