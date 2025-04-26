module HelloTriangle;
import Scene;

struct VertexInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORDINATE;
    float3 Normal : NORMAL;
    float4 Color : COLOR;
};

typedef struct VertexOutput
{
    float4 Position : SV_Position;
    float2 TexCoord;
    float3 Normal;
    float4 Color;
} FragmentInput;

ConstantBuffer<Camera> InCamera;
ConstantBuffer<Model> InModel;


[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Out;
    Out.Position = mul(InCamera.Projection * InCamera.View * InModel.Model, float4(Input.Position, 1.0));
    Out.Position.z = (Out.Position.z + Out.Position.w) * 0.5;
    Out.TexCoord = Input.TexCoord;
    Out.Normal = Input.Normal;
    Out.Color = Input.Color;
    return Out;
}

[shader("fragment")]
float4 FragmentMain(FragmentInput Input) : SV_Target
{
    return Input.Color;
}