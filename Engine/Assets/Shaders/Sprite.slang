module Sprite;

struct VertexInput
{
	float2 position;
    float2 texCoords;
}

struct VertexOutput
{
	float4 position : SV_Position;
    float2 texCoords;
}

struct Uniforms
{
    float4x4 mvp;
	float2 tiling;
    float2x2 spriteScale;
    float4 color;
}

[vk::binding(0, 0)] SamplerState sampler;
[vk::binding(1, 0)] Texture2D texture;
[vk::binding(2, 0)] ConstantBuffer<Uniforms, Std140DataLayout> uniforms;

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
	VertexOutput out;
	float2 scaledPosition = mul(uniforms.spriteScale, input.position);
    out.position = mul(uniforms.mvp, float4(scaledPosition, 0.0, 1.0));
    out.texCoords = uniforms.tiling * input.texCoords;
	return out;
}

[shader("fragment")]
float4 frag(VertexOutput input) : SV_Target
{
    float4 sample = texture.Sample(sampler, input.texCoords);
    float4 color = sample * uniforms.color;
	if (color.a < 0.01)
		discard;
	return color;
}