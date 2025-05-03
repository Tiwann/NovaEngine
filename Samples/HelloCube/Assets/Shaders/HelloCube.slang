module HelloTriangle;
import Common;

struct VertexInput
{
    vec3 Position : POSITION;
    vec2 TexCoord : TEXCOORDINATE;
    vec3 Normal : NORMAL;
    vec4 Color : COLOR;
};

typedef struct VertexOutput
{
    vec4 Position : SV_Position;
    vec2 TexCoord;
    vec3 Normal;
    vec4 Color;
} FragmentInput;

struct EntityData
{
    mat4 Model;
}

ConstantBuffer<CameraData> Camera;
ConstantBuffer<EntityData> Entity;


[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Out;
    mat4 ModelView = mul(Camera.View, Entity.Model);
    mat4 ModelViewProjection = mul(Camera.Projection, ModelView);
    Out.Position = mul(ModelViewProjection, vec4(Input.Position, 1.0));
    Out.TexCoord = Input.TexCoord;
    Out.Normal = remap(Input.Normal, -1.0, 1.0, 0.0, 1.0);
    Out.Color = Input.Color;
    return Out;
}

[shader("fragment")]
vec4 FragmentMain(FragmentInput Input) : SV_Target
{
    return vec4(Input.Normal, 1.0);
}