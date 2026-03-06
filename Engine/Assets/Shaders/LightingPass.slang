module LightingPass;
import FullScreenVertex;

[[vk::binding(0, 0)]] Sampler2D<float4> albedo;
[[vk::binding(0, 1)]] Sampler2D<float4> position;
[[vk::binding(0, 2)]] Sampler2D<float4> normal;
[[vk::binding(0, 3)]] Sampler2D<float4> specGloss;

[shader("fragment")]
float4 frag(FragmentInput input) : SV_Target
{
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

    float specularMap = specularTex.Sample(sampler, input.texCoord).r;
    float glossinessMap = glossinessTex.Sample(sampler, input.texCoord).r;
    float glossinessMap = 0.5;
    float3 specular = pow(max(dot(N, H), 0.0), exp2(glossinessMap * 11.0 + 2.0));
    float3 specularLight = specular * dirLight;

    float3 finalColor = diffuseLight + specularLight;
	return float4(finalColor, 1.0);
}

