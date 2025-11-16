module BlinnPhong;
import VertexData3D;
import Common;

[[vk::binding(0, 0)]] ConstantBuffer<Nova::Scene, Std140DataLayout> scene;
[[vk::binding(1, 0)]] SamplerState sampler;

[[vk::binding(0, 1)]] Texture2D albedoTex;

[shader("vertex")]
VertexOutput vert(VertexInput input) {
	VertexOutput out;
	out.position = scene.LocalToClipPosition(float4(input.position, 1.0));
	out.texCoord = input.texCoord;
	out.normal = scene.LocalToWorldNormal(input.normal);
	out.color = input.color;
	out.fragPos = (scene.modelMatrix * float4(input.position, 1.0)).xyz;
	return out;
}

[shader("fragment")]
float4 frag(FragmentInput input) : SV_Target {
	float3 lightColor = scene.dirLightColor * scene.dirLightIntensity;
	float3 N = normalize(input.normal);
	float3 L = normalize(scene.dirLightDir);

    float3 albedo = albedoTex.Sample(sampler, input.texCoord).xyz;
    float diffuse = max(dot(N, L), 0.0);
    float3 diffuseLight = diffuse * lightColor * albedo;
	float3 ambientLight = scene.ambLightColor * scene.ambLightIntensity;

    float3 V = normalize(scene.cameraWorldPos - input.fragPos);
	float3 H = normalize(L + V);
	float specular = max(dot(H, N), 0.0);
	float glossiness = 0.5;
	float specularExponent = exp2(glossiness * 11.0) + 2.0;
	specular = pow(specular, specularExponent);

	float3 finalColor = ambientLight + diffuseLight + specular;
	return float4(albedo, 1.0);
}
