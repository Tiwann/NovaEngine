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
    int32_t frameIndex;
    int32_t cols;
    int32_t rows;
    int32_t flags;
    float4x4 mvp;
	float2 tiling;
    float2x2 spriteScale;
    float4 color;
}

[vk::binding(0, 0)] SamplerState sampler;
[vk::binding(1, 0)] Texture2D texture;
[vk::binding(2, 0)] ConstantBuffer<Uniforms, Std140DataLayout> uniforms;

enum Flags
{
	None = 0,
	FlipHorizontal = 1 << 1,
	FlipVertical = 1 << 2,
}

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
    VertexOutput out;
    uint textureWidth = 0, textureHeight = 0;
    texture.GetDimensions(textureWidth, textureHeight);

    const int col = uniforms.frameIndex % uniforms.cols;
    const int row = uniforms.frameIndex / uniforms.cols;

    const float2 frameSize = float2(1.0 / uniforms.cols, 1.0 / uniforms.rows);
    float2 texCoords = input.texCoords * frameSize + float2(col, row) * frameSize;
    if ((uniforms.flags & Flags.FlipHorizontal) != 0)
        texCoords.x *= -1;

    if ((uniforms.flags & Flags.FlipVertical) != 0)
            texCoords.y *= -1;

	float2 scaledPosition = mul(uniforms.spriteScale, input.position);
    out.position = mul(uniforms.mvp, float4(scaledPosition, 0.0, 1.0));
    out.texCoords = uniforms.tiling * texCoords;
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