module HelloTriangle;

struct VertexInput
{
    float3 Position;
    float2 TexCoord;
    float3 Normal;
    float4 Color;
};

typedef struct VertexOutput
{
    float4 Position : SV_Position;
    float2 TexCoord;
    float3 Normal;
    float4 Color;
} FragmentInput;

/*
struct CameraData
{
    float4x4 View;
    float4x4 Projection;
};

struct ModelData
{
    float4x4 Transform;
}

ConstantBuffer<CameraData> Camera;
ConstantBuffer<ModelData> Model;
 */


[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Out;
    Out.Position = float4(Input.Position, 1.0);
    Out.Position.z = (Out.Position.z + Out.Position.w) * 0.5;
    Out.TexCoord = Input.TexCoord;
    Out.Normal = Input.Normal;
    Out.Color = Input.Color;
    return Out;
}

[shader("fragment")]
float4 FragmentMain(FragmentInput Input)
{
    return Input.Color;
}