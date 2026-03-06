module BlinnPhong;
import Common;
import Object;
import Camera;
import Scene;
import LightTypes;

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
	float3 albedoColor;
	float specular;
	float glossiness;
#ifdef NOVA_MATERIAL_TRANSPARENT
	float opacity;
#else
#ifdef NOVA_MATERIAL_CUTOUT
	float opacity;
	float alphaCutoff;
#endif
#endif
}

[[vk::binding(0, 0)]] ConstantBuffer<MaterialParameters> material;
[[vk::binding(1, 0)]] Sampler2D diffuseTex;
[[vk::binding(2, 0)]] Sampler2D specularTex;
[[vk::binding(3, 0)]] Sampler2D glossinessTex;
[[vk::binding(4, 0)]] Sampler2D normalTex;

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
    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.bitangent);

    float3x3 TBN = float3x3(T, B, N);

    float4 albedoMap = diffuseTex.Sample(input.texCoord);
    float specMap = specularTex.Sample(input.texCoord).r;
    float glossMap = glossinessTex.Sample(input.texCoord).r;
    float3 normalMap = normalTex.Sample(input.texCoord).rgb;

#ifdef NOVA_MATERIAL_CUTOUT
	float finalOpacity = albedoMap.a * material.opacity;
	if(finalOpacity < material.alphaCutoff)
		discard;
#endif

    float3 tangentNormal = normalize(normalMap * 2.0 - 1.0);
    float3 worldNormal = normalize(TBN * tangentNormal);

    DirectionalLight dirLight = u_SceneData.directionalLight;
    AmbientLight ambLight = u_SceneData.ambientLight;

    float3 lightDir = normalize(-dirLight.direction);
    float3 viewDir = normalize(u_CameraData.cameraPos - input.worldPos);
    float3 halfDir = normalize(lightDir + viewDir);

    float NdotL = max(dot(worldNormal, lightDir), 0.0);
    float3 diffuse = albedoMap.rgb * material.albedoColor.rgb * dirLight.color * NdotL;

    float shininess = max(glossMap * material.glossiness * 256.0, 1.0);
    float NdotH = max(dot(worldNormal, halfDir), 0.0);
    float specularFactor = pow(NdotH, shininess);

    float3 specular = dirLight.color * specularFactor * specMap * material.specular;
    float3 ambient = albedoMap.rgb * ambLight.color;
    float3 finalColor = ambient + diffuse + specular;

#ifdef NOVA_MATERIAL_TRANSPARENT
    return float4(finalColor, material.opacity);
#else
	return float4(finalColor, 1.0);
#endif
}
