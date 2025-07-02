module Font;
import Common;

struct VertexInput
{
  float3 Position : POSITION;
  float2 TexCoord : TEXCOORD;
  float3 Normal : NORMAL;
  float4 Color : COLOR;
}

typedef struct VertexOutput
{
  float4 Position : SV_Position;
  float2 TexCoord;
  float4 Color;
} FragmentInput;

public struct SceneData
{
  public float4x4 Model;
  public float4x4 View;
  public float4x4 Projection;
}

ConstantBuffer<SceneData> Scene;
Sampler2D AtlasTexture;

[shader("vertex")]
VertexOutput VertexMain(VertexInput Input)
{
	VertexOutput Out;
    Out.Position = mul(mul(mul(Scene.Projection, Scene.View), Scene.Model), float4(Input.Position, 1.0));
    Out.TexCoord = Input.TexCoord;
    Out.Color = Input.Color;
    return Out;
}

float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

float median(float3 a)
{
    return max(min(a.r, a.g), min(max(a.r, a.g), a.b));
}

float median(float4 a)
{
    return median(a.rgb);
}


float ScreenPixelRange(float PixelRange, float2 TexCoord)
{
    uint width = 0, height = 0;
    AtlasTexture.GetDimensions(0, width, height, 1);

    float2 unitRange = float2(PixelRange) / float2((float)width, (float)height);
    float2 screenTexSize = float2(1.0) / fwidth(TexCoord);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

[shader("fragment")]
float4 FragmentMain(FragmentInput Input) : SV_Target
{
	float4 sampledAtlas = AtlasTexture.Sample(Input.TexCoord);
	float sd = median(sampledAtlas);
	float screenPxDistance = ScreenPixelRange(1.0, Input.TexCoord) * (sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	return lerp(float4(0.0), float4(1.0), opacity);
}