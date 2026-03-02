module Camera;

public struct Camera
{
	public float4x4 viewMatrix;
	public float4x4 inverseViewMatrix;
	public float4x4 projectionMatrix;
    public float4x4 inverseProjectionMatrix;
	public float4x4 viewProjectionMatrix;
	public float4x4 inverseViewProjectionMatrix;
};

[[vk::binding(0, 2)]] ConstantBuffer<Camera> u_Camera;