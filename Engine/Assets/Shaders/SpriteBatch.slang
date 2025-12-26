module SpriteBatch;

struct VertexInput
{
	float2 position
    float2 texCoords;
}

struct VertexOutput
{
	float4 position : SV_Position;
    float2 texCoords;
}

static const VertexInput input[] =
{
    {float2(-1.0, +1.0), float2(0.0, 1.0)},
    {float2(+1.0, +1.0), float2(1.0, 1.0)},
    {float2(+1.0, -1.0), float2(1.0, 0.0)},
    {float2(-1.0, -1.0), float2(0.0, 0.0)},
};

static const uint indices[] = { 0, 2, 1, 0, 3, 2 };

struct SpriteData
{
    int32_t frameIndex;
    int32_t cols;
    int32_t rows;
    int32_t flags;
    float2 tiling;
    float2 offset;
    float2x2 spriteScale;
    float4 color;
    uint32_t textureIndex;
}

[vk::binding(0, 0)] SamplerState sampler;
[vk::binding(1, 0)] Texture2D textures[];
[vk::binding(2, 0)] uniform SpriteData spriteData[];

[[vk::push_constant]] float4x4 mvp;

enum Flags
{
	None = 0,
	FlipHorizontal = 1 << 1,
	FlipVertical = 1 << 2,
}

[shader("vertex")]
VertexOutput vert(uint vertexID : SV_VertexID)
{
    uint index = indices[vertexID];

    VertexOutput output;
    float2 texCoords = input[index].texCoords;
	if(spriteData.cols != 0 && spriteData.rows != 0)
	{
        const int col = spriteData.frameIndex % spriteData.cols;
        const int row = spriteData.frameIndex / spriteData.cols;

		const float2 frameSize = float2(1.0 / spriteData.cols, 1.0 / spriteData.rows);
        texCoords = input[index].texCoords * frameSize + float2(col, row) * frameSize;

        if ((spriteData.flags & Flags.FlipHorizontal) == 0)
            texCoords.x *= -1;

        if ((spriteData.flags & Flags.FlipVertical) == 0)
                texCoords.y *= -1;
    }


	float2 scaledPosition = mul(spriteData.spriteScale, input[index].position);
    output.position = mul(mvp, float4(scaledPosition, 0.0, 1.0));
    output.texCoords = spriteData.tiling * texCoords + spriteData.offset;
	return out;
}

[shader("fragment")]
float4 frag(VertexOutput input) : SV_Target
{
    uint32_t textureIndex = spriteData.textureIndex;
    float4 sample = textures[textureIndex].Sample(sampler, input.texCoords);
    float4 color = sample * spriteData.color;

	if (color.a < 0.01)
		discard;

	return color;
}