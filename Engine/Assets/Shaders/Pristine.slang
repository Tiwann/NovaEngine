module Pristine;

public float PristineGrid(float2 TexCoord, float Width)
{
    Width = saturate(Width);
    float4 TexCoordDDXY = float4(ddx(TexCoord), ddy(TexCoord));
    float2 uvDeriv = float2(length(TexCoordDDXY.xz), length(TexCoordDDXY.yw));
    bool InvertLine = Width > 0.5;
    float2 TargetWidth = InvertLine ? 1.0 - Width : Width;
    float2 DrawWidth = clamp(TargetWidth, uvDeriv, 0.5);
    float2 LineAntiAliasing = max(uvDeriv, 0.000001) * 1.5;
    float2 GridUV = abs(frac(TexCoord) * 2.0 - 1.0);
    GridUV = InvertLine ? GridUV : 1.0 - GridUV;
    float2 Grid = smoothstep(DrawWidth + LineAntiAliasing, DrawWidth - LineAntiAliasing, GridUV);
    Grid *= saturate(TargetWidth / DrawWidth);
    Grid = lerp(Grid, TargetWidth, saturate(uvDeriv * 2.0 - 1.0));
    Grid = InvertLine ? 1.0 - Grid : Grid;
    return lerp(Grid.x, 1.0, Grid.y);
}
