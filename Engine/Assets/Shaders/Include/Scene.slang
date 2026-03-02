module Scene;
import LightTypes;

public struct Scene
{
	DirectionalLight directionalLight;
	AmbientLight ambientLight;
	uint32_t pointLightCount;
	ConstantBuffer<PointLight> pointLights;
	uint32_t spotLightCount;
	ConstantBuffer<SpotLight> spotLights;
}

[[vk::binding(1, 2)]] ConstantBuffer<Scene> u_Scene;


