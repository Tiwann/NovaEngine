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

struct CameraData
{
    mat4 View;
    mat4 Projection;
}

struct EntityData
{
    mat4 Model;
}

ConstantBuffer<CameraData> Camera;
ConstantBuffer<EntityData> Entity;
Sampler2D Texture;


[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
    VertexOutput Out;
    Out.Position = ConvertToClipSpace(Camera, Entity.Model, vec4(Input.Position, 1.0));
    Out.TexCoord = Input.TexCoord;
    Out.Normal = Input.Normal;
    Out.Color = Input.Color;
    return Out;
}

[shader("fragment")]
vec4 FragmentMain(FragmentInput Input) : SV_Target
{
    vec4 Color = Texture.Sample(Input.TexCoord);
    return Color * Input.Color;
}