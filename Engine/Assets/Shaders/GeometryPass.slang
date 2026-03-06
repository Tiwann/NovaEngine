module GeometryPass;
import Common;
import Scene;
import Object;
import Camera;

struct VertexInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORDINATE;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 color : COLOR;
};

struct GBuffer
{
	float4 albedo : SV_TARGET0;
	float4 position : SV_TARGET1;
	float4 normal : SV_TARGET2;
	float4 specGloss : SV_TARGET3;
}

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 texCoord;
    float3 normal;
    float3 tangent;
    float3 bitangent;
    float4 color;
    float3 worldPos;
};

[[vk::binding(0, 1)]] Sampler2D diffuseTex;
[[vk::binding(0, 2)]] Sampler2D normalTex;
[[vk::binding(0, 3)]] Sampler2D specularTex;
[[vk::binding(0, 4)]] Sampler2D glossinessTex;

[shader("vertex")]
VertexOutput vert(VertexInput input)
{
	VertexOutput out;
	out.position = localToClipPosition(float4(input.position, 1.0));
	out.texCoord = input.texCoord;
    out.normal = localToWorldNormal(input.normal);
    out.tangent = localToWorldDirection(input.tangent);
    out.bitangent = cross(out.normal, out.tangent);
    out.color = input.color;
    out.worldPos = localToWorldPosition(float4(input.position, 1.0)).xyz;
	return out;
}

[shader("fragment")]
GBuffer frag(VertexOutput input)
{
    GBuffer gBuffer;
    gBuffer.albedo = diffuseTex.Sample(input.texCoord) * input.color;
    gBuffer.position = float4(input.worldPos, 1.0);

    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 bitangent = normalize(input.bitangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);

    float3 sampledNormal = normalTex.Sample(input.texCoord).xyz * 2.0 - 1.0;
    gBuffer.normal = float4(normalize(TBN * sampledNormal), 0.0);

	float4 specularMap = specularTex.Sample(input.texCoord);
	float4 glossinessMap = glossinessTex.Sample(input.texCoord);
	gBuffer.specGloss.r = specularMap.r;
	gBuffer.specGloss.g = glossinessMap.r;
	gBuffer.specGloss.b = 0.0;
	gBuffer.specGloss.a = 1.0;
	return gBuffer;
}
