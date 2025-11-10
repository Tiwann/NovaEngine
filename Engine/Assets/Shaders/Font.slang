module Font;
import Common;

struct VertexInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
}

typedef struct VertexOutput
{
  float4 position : SV_Position;
  float2 texCoord;
  float4 color;
} FragmentInput;

[[vk::push_constant]] ConstantBuffer<float4x4> mvp;
[[vk::binding(0, 0)]] Sampler2D fontAtlas;


[shader("vertex")]
VertexOutput vert(VertexInput input)
{
	VertexOutput out;
    out.position = mvp * float4(input.position, 1.0);
    out.texCoord = input.texCoord;
    out.color = input.color;
    return out;
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float screenPixelRange(float pixelRange, float2 texCoord)
{
    float width = 0, height = 0;
    fontAtlas.GetDimensions(width, height);

    float2 unitRange = float2(pixelRange) / float2(width, height);
    float2 screenTexSize = float2(1.0) / fwidth(texCoord);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

[shader("fragment")]
float4 frag(FragmentInput input) : SV_Target
{
	float4 sampledAtlas = fontAtlas.Sample(input.texCoord);
	float sd = median(sampledAtlas.r, sampledAtlas.g, sampledAtlas.b);
	float screenPxDistance = screenPixelRange(2.0, input.texCoord) * (sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	return lerp(float4(1.0, 1.0, 1.0, 0.0), float4(1.0, 1.0, 1.0, 1.0), opacity);
}