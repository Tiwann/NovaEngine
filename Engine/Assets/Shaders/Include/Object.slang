module Object;

public struct ObjectData
{
	public float4x4 worldSpaceMatrix;
	public float4x4 normalMatrix;
}

[vk::binding(0, 1)] public ConstantBuffer<ObjectData> u_ObjectData : register(b0, space1);