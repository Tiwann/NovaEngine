module Scene;
import LightTypes;

public struct SceneData
{
	public DirectionalLight directionalLight;
	public AmbientLight ambientLight;
	public uint32_t pointLightCount;
	public uint32_t spotLightCount;
}

[vk::binding(1, 2)] public ConstantBuffer<SceneData> u_SceneData : register(b1, space2);

