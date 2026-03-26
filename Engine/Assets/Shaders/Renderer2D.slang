module Renderer2D;
import Common;

enum QuadMode : uint32_t
{
	Quad,
    Ellipse
}

struct VertexInput
{
	float2 position : POSITION;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
	QuadMode mode : MODE;
}

struct VertexOutput
{
	float4 position : SV_POSITION;
	float2 texCoord;
	float4 color;
	nointerpolation QuadMode mode;
}

[vk::push_constant] ConstantBuffer<float4x4> viewProj;

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
	VertexOutput output;
	output.position = viewProj * float4(input.position, 0.0f, 1.0);
	output.texCoord = input.texCoord;
	output.color = input.color;
	output.mode = input.mode;
	return output;
}

float ellipseMask(float2 uv)
{
	// distance is using sqrt. optimize by using squared distance.
	// achieving that using dot(x, x)
	float2 center = (uv - float2(0.5, 0.5)) * 2.0;
	float length = dot(center, center);
	float mask = 1.0 - smoothstep(0.97*0.97, 0.99*0.99, length);
	return saturate(mask);
}

float4 drawEllipse(float2 uv, float4 color)
{
	return color * ellipseMask(uv);
}

[shader("fragment")]
float4 frag(VertexOutput input) : SV_TARGET
{
	switch(input.mode)
	{
	case QuadMode::Quad: return input.color;
	case QuadMode::Ellipse: return drawEllipse(input.texCoord, input.color);
	}

	return float4(1.0, 0.03, 1.0, 1.0);
}