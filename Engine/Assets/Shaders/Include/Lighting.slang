module Lighting;
import PBRCommon;
import LightTypes;
import Common;

public float3 CalculateDirLight(DirectionalLight light, float3 F0, float3 N, float3 V, float3 albedo, float metalness, float roughness)
{
    float3 result = float3(0.0);

    float3 Li = light.direction;
    float3 Lradiance = light.color * light.intensity;
    float3 Lh = normalize(Li + V);
    float NdotV = max(dot(N, V), 0.0);

    // Calculate angles between surface normal and various light vectors.
    float cosLi = max(0.0, dot(N, Li));
    float cosLh = max(0.0, dot(N, Lh));

    float3 F = FresnelSchlickRoughness(F0, max(0.0, dot(Lh, V)), roughness);
    float D = NdfGGX(cosLh, roughness);
    float G = GaSchlickGGX(cosLi, NdotV, roughness);

    float3 kd = (1.0 - F) * (1.0 - metalness);
    float3 diffuseBRDF = kd * albedo;

    // Cook-Torrance
    float3 specularBRDF = (F * D * G) / max(Constants<float>::Epsilon, 4.0 * cosLi * NdotV);
    specularBRDF = clamp(specularBRDF, float3(0.0f), float3(10.0f));
    result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
	return result;
}