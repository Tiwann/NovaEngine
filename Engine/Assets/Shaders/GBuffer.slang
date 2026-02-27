module GBuffer;
import Common;

public struct VertexInput
{
    public float3 position : POSITION;
    public float2 texCoord : TEXCOORDINATE;
    public float3 normal : NORMAL;
    public float3 tangent : TANGENT;
    public float4 color : COLOR;
};

public struct GBuffer
{
	public float4 albedo : SV_Target0;
	public float4 position : SV_Target1;
	public float4 normal : SV_Target2;
	public float4 specGloss : SV_Target3;
}

public struct VertexOutput
{
    public float4 position : SV_Position;
    public float2 texCoord;
    public float3 normal;
    public float3 tangent;
    public float3 bitangent;
    public float4 color;
    public float3 worldPos;
};

[[vk::binding(0, 0)]] ConstantBuffer<Nova::Scene, Std140DataLayout> scene;
[[vk::binding(0, 1)]] Sampler2D diffuseTex;
[[vk::binding(0, 2)]] Sampler2D normalTex;
[[vk::binding(0, 3)]] Sampler2D specularTex;
[[vk::binding(0, 4)]] Sampler2D glossinessTex;

[shader("vertex")]
VertexOutput vert(VertexInput input) 
{
	VertexOutput out;
	out.position = scene.LocalToClipPosition(float4(input.position, 1.0));
	out.texCoord = input.texCoord;
    out.normal = scene.LocalToWorldNormal(input.normal);
    out.tangent = scene.LocalToWorldDirection(input.tangent);
    out.bitangent = cross(out.normal, out.tangent);
    out.color = input.color;
    out.worldPos = scene.LocalToWorldPosition(float4(input.position, 1.0)).xyz;
	return out;
}

[shader("fragment")]
GBuffer frag(VertexOutput input)
{
    GBuffer gBuffer;
    gBuffer.albedo = diffuseTex.Sample(input.texCoord);
    gBuffer.position = input.position;
	gBuffer.normal = normalTex.Sample(input.texCoord);
	float4 specularMap = specularTex.Sample(input.texCoord);
	float4 glossinessMap = glossinessTex.Sample(input.texCoord);
	gBuffer.specGloss.r = specularMap.r;
	gBuffer.specGloss.g = glossinessMap.r;
	return gBuffer;
}
