module LightTypes;

public struct DirectionalLight
{
    public float3 color;
	public float intensity;
	public float3 direction;
};

public struct AmbientLight
{
    public float3 color;
	public float intensity;
};

public struct PointLight
{
	public float3 position;
	public float3 color;
	public float intensity;
	public float innerRadius;
	public float outerRadius;
};

public struct SpotLight
{
	public float3 position;
	public float3 direction;
	public float angleAttenuation;
	public float3 radiance;
	public float range;
	public float angle;
	public float falloff;
};