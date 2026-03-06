module Camera;

public struct CameraData
{
	public float4x4 viewMatrix;
	public float4x4 inverseViewMatrix;
	public float4x4 projectionMatrix;
    public float4x4 inverseProjectionMatrix;
	public float4x4 viewProjectionMatrix;
	public float4x4 inverseViewProjectionMatrix;
	public float3 cameraPos;
	public float3 cameraDir;
};

[vk::binding(0, 2)] public ConstantBuffer<CameraData> u_CameraData : register(b0, space2);