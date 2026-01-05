module SpriteBatch;

struct VertexInput
{
	float2 position;
	float2 texCoords;
}

struct VertexOutput
{
	float4 position : SV_Position;
	float2 texCoords;
	float4 color;
	uint32_t spriteId;
}

static const VertexInput input[] =
{
	{float2(-1.0, +1.0), float2(0.0, 1.0)},
	{float2(+1.0, +1.0), float2(1.0, 1.0)},
	{float2(+1.0, -1.0), float2(1.0, 0.0)},
	{float2(-1.0, -1.0), float2(0.0, 0.0)},
};

static const uint32_t indices[] = { 0, 2, 1, 0, 3, 2 };

struct SpriteData
{
	uint32_t id;
	int32_t frameIndex;
	int32_t cols;
	int32_t rows;
	int32_t flags;
	int32_t padding0;
	float2 tiling;
	float2 offset;
	float2x2 scale;
	float4 color;
	float4x4 worldToClip;
}

[vk::binding(0, 0)] SamplerState sampler;
[vk::binding(1, 0)] Sampler2D textures[1024];
[vk::binding(2, 0)] StructuredBuffer<SpriteData> sprites;

enum Flags
{
	None = 0,
	FlipHorizontal = 1 << 0,
	FlipVertical = 1 << 1,
}

[shader("vertex")]
VertexOutput vert(uint32_t vertexID : SV_VertexID, uint32_t instanceID : SV_InstanceID)
{
	uint32_t vertexIndex = indices[vertexID];
	SpriteData sprite = sprites[instanceID];

	float2 inputPosition = input[vertexIndex].position;
	float2 texCoords = input[vertexIndex].texCoords;

	if(sprite.cols != 0 && sprite.rows != 0)
	{
		const int col = sprite.frameIndex % sprite.cols;
		const int row = sprite.frameIndex / sprite.cols;

		const float2 frameSize = float2(1.0 / sprite.cols, 1.0 / sprite.rows);
		texCoords = input[vertexIndex].texCoords * frameSize + float2(col, row) * frameSize;

		if ((sprite.flags & Flags.FlipHorizontal) != 0)
			texCoords.x = 1.0 - texCoords.x;

		if ((sprite.flags & Flags.FlipVertical) != 0)
			texCoords.y = 1.0 - texCoords.y;
	}

	float2 scaledPosition = mul(sprite.scale, inputPosition);

	VertexOutput output;
	output.position = mul(sprite.worldToClip, float4(scaledPosition, 0.0, 1.0));
	output.texCoords = sprite.tiling * texCoords + sprite.offset;
	output.color = sprite.color;
	output.spriteId = sprite.id;
	return output;
}

[shader("fragment")]
float4 frag(VertexOutput input) : SV_Target
{
	float4 sample = textures[NonUniformResourceIndex(input.spriteId)].Sample(input.texCoords);
	float4 color = sample * input.color;

	if (color.a < 0.01)
		discard;

	return color;
}