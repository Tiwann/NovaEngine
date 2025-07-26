module HelloCompute;

RWTexture2D<float4> screen;

[shader("compute")]
[numthreads(16, 16, 1)]
void computeMain(uint3 threadId : SV_DispatchThreadID)
{
    uint2 screenPos = threadId.xy;
    uint texWidth = 0, texHeight = 0;
    screen.GetDimensions(texWidth, texHeight);
    if (screenPos.x >= texWidth || screenPos.y >= texHeight)
        return;

    float2 texSize = float2((float)texWidth, (float)texHeight);
    float2 normalizedCoord = float2((float)screenPos.x, (float)screenPos.y) / texSize;
    float4 color = float4(normalizedCoord, 0.0, 1.0);
    screen[screenPos] = color;
}