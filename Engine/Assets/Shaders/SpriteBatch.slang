module Sprite;

struct VertexInput
{
	float2 position;
    float2 texCoords;
    uint spriteId;
    uint quadId;
}

struct VertexOutput
{
	float4 position : SV_Position;
    float2 texCoords;
    uint spriteId;
    uint quadId;
}

struct Uniforms
{
    float4x4 transform;
	float2 tiling;
	float2x2 spriteScale;
	float4 color;
}

[vk::binding(0, 0)] StructuredBuffer<Uniforms, Std140DataLayout> uniforms;
[vk::binding(1, 0)] Sampler2D textures[];


[shader("vertex")]
VertexOutput vert(VertexInput input)
{
	VertexOutput out;
	float2 scaledPosition = mul(uniforms[input.quadId].spriteScale, input.position);
	float4 position = mul(uniforms[input.quadId].transform, float4(scaledPosition, 0.0, 1.0));
	out.position = position;
    out.texCoords = uniforms[input.quadId].tiling * input.texCoords;
    out.spriteId = input.spriteId;
    out.quadId = input.quadId;
	return out;
}

[shader("fragment")]
float4 frag(VertexOutput input) : SV_Target
{
	float4 sample = textures[input.quadId].Sample(input.texCoords);
	if (sample.a < 0.01)
		discard;
	return sample;
}