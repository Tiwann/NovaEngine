module PBRShading;
import PBRCommon;
import Common;
import Object;
import Camera;
import Scene;
import LightTypes;
import Lighting;

public struct VertexInput
{
    public float3 position : POSITION;
    public float2 texCoord : TEXCOORDINATE;
    public float3 normal : NORMAL;
    public float3 tangent : TANGENT;
    public float4 color : COLOR;
};

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

struct MaterialParameters
{
	float3 albedo;
	float metallness;
	float roughness;
	float3 emissive;
#ifdef NOVA_MATERIAL_TRANSPARENT
	float opacity;
#else
#ifdef NOVA_MATERIAL_CUTOUT
	float opacity;
	float alphaCutoff;
#endif
#endif
}

static const float3 Fdielectric = float3(0.04);

[vk::push_constant] uniform MaterialParameters materialParams;
[vk::binding(1, 1)] SamplerState u_Sampler : register(s1, space1);

[vk::binding(0, 0)] Texture2D albedoTex : register(t0, space0);
[vk::binding(1, 0)] Texture2D metallnessRoughnessTex : register(t1, space0);
[vk::binding(2, 0)] Texture2D normalTex : register(t2, space0);
[vk::binding(3, 0)] Texture2D emissiveTex : register(t3, space0);


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
float4 frag(VertexOutput input) : SV_Target
{
	float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 bitangent = normalize(input.bitangent);

    float3x3 tangentToWorldNormals = float3x3(tangent, bitangent, normal);

    float4 albedoMap = albedoTex.Sample(u_Sampler, input.texCoord);
    float4 metalnessRoughnessMap = metallnessRoughnessTex.Sample(u_Sampler, input.texCoord);
    float metallnessMap = metalnessRoughnessMap.b;
    float roughnessMap = metalnessRoughnessMap.g;
    float3 normalMap = normalTex.Sample(u_Sampler, input.texCoord).rgb * 2.0 - 1.0;

#ifdef NOVA_MATERIAL_CUTOUT
    float finalOpacity = albedoMap.a * materialParams.opacity;
    if (finalOpacity < materialParams.alphaCutoff)
        discard;
#endif

    float4 albedo = albedoMap * float4(srgbToLinear(materialParams.albedo), 1.0);
    float metalness = metallnessMap * materialParams.metallness;
    float roughness = roughnessMap * materialParams.roughness;
    float3 N = normalize(tangentToWorldNormals * normalMap);
    float3 V = normalize(u_CameraData.cameraPos - input.worldPos);
    float NdotV = max(dot(N, V), 0.0);
    
	float3 L = 2.0 * NdotV * N - V;
	float3 F0 = lerp(Fdielectric, albedo.rgb, metalness);

	DirectionalLight light = u_SceneData.directionalLight;
	AmbientLight ambientLight = u_SceneData.ambientLight;
    float3 lightContribution = CalculateDirLight(light, F0, N, V, albedo.rgb, metalness, roughness);
    float3 ambient = albedoMap.rgb * ambientLight.color * ambientLight.intensity;
	float4 finalColor = float4(lightContribution + ambient, 1.0);

#ifdef NOVA_MATERIAL_TRANSPARENT
    return float4(finalColor.rgb, materialParams.opacity);
#else
    return float4(finalColor.rgb, 1.0);
#endif
}