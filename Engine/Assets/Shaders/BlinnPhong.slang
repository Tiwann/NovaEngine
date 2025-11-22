module BlinnPhong;
import Common;

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

public typedef VertexOutput FragmentInput;

[[vk::binding(0, 0)]] ConstantBuffer<Nova::Scene, Std140DataLayout> scene;
[[vk::binding(1, 0)]] SamplerState sampler;

[[vk::binding(0, 1)]] Texture2D diffuseTex;
//[[vk::binding(1, 1)]] Texture2D specularTex;
//[[vk::binding(2, 1)]] Texture2D glossinessTex;
//[[vk::binding(3, 1)]] Texture2D normalTex;

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
float4 frag(FragmentInput input) : SV_Target
{
    //float3x3 tangentToWorld = constructSpace(input.tangent, input.bitangent, input.normal);
//
    //float4 normalMap = normalTex.Sample(sampler, input.texCoord);
    //if (normalMap.a == 0)
    //    normalMap = float4(0.5, 0.5, 1, 0);
//
    //float3 unpackedNormalMap = normalMap.rgb * 2.0 - 1.0;

    //float3 N = tangentToWorld * unpackedNormalMap;
    float3 N = normalize(input.normal);
	float3 L = normalize(scene.dirLightDir);
	float3 V = normalize(scene.cameraWorldPos - input.worldPos);
    float3 H = normalize(L + V);
    float3 R = reflect(-L, N);

    float3 dirLight = scene.dirLightColor * scene.dirLightIntensity;
    float3 ambientLight = scene.ambLightColor * scene.ambLightIntensity;

    float diffuse = max(dot(N, L), 0.0);
    float4 diffuseMap = diffuseTex.Sample(sampler, input.texCoord);
    float3 diffuseLight = (diffuse + ambientLight) * diffuseMap.rgb * dirLight;

    // float specularMap = specularTex.Sample(sampler, input.texCoord).r;
    // float glossinessMap = glossinessTex.Sample(sampler, input.texCoord).r;
    float glossinessMap = 0.5;
    float3 specular = pow(max(dot(N, H), 0.0), exp2(glossinessMap * 11.0 + 2.0));
    float3 specularLight = specular * dirLight;

    float3 finalColor = diffuseLight + specularLight;
	return float4(finalColor, 1.0);
}
