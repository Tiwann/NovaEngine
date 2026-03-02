module Screen;

public struct ScreenData
{
    float2 FullResolution;
    float2 HalfResolution;
};

[[vk::binding(1, 2)]] ConstantBuffer<ScreenData> u_ScreenData;