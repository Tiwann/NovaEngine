module Common;

public float2 operator*(float2x2 lhs, float2 rhs)
{
    return mul(lhs, rhs);
}

public float3 operator*(float3x3 lhs, float3 rhs)
{
    return mul(lhs, rhs);
}

public float4 operator*(float4x4 lhs, float4 rhs)
{
    return mul(lhs, rhs);
}

public float3 operator*(float3x3 lhs, float4 rhs)
{
    return mul(lhs, rhs.xyz);
}


public float4 operator*(float4x4 lhs, float3 rhs)
{
    return mul(lhs, float4(rhs, 1.0));
}

public float4 operator*(float4x4 lhs, float2 rhs)
{
    return mul(lhs, float4(rhs, 0.0, 1.0));
}

public float3 operator*(float3x3 lhs, float2 rhs)
{
    return mul(lhs, float3(rhs, 1.0));
}

public float remap(float val, float inmin, float inmax, float outmin, float outmax)
{
    return outmin + (outmax - outmin) * ((val - inmin) / (inmax - inmin));
}

public float2 remap(float2 val, float inmin, float inmax, float outmin, float outmax)
{
    float x = remap(val.x, inmin, inmax, outmin, outmax);
    float y = remap(val.y, inmin, inmax, outmin, outmax);
    return float2(x, y);
}

public float3 remap(float3 val, float inmin, float inmax, float outmin, float outmax)
{
    float x = remap(val.x, inmin, inmax, outmin, outmax);
    float y = remap(val.y, inmin, inmax, outmin, outmax);
    float z = remap(val.z, inmin, inmax, outmin, outmax);
    return float3(x, y, z);
}

public float4 remap(float4 val, float inmin, float inmax, float outmin, float outmax)
{
    float x = remap(val.x, inmin, inmax, outmin, outmax);
    float y = remap(val.y, inmin, inmax, outmin, outmax);
    float z = remap(val.z, inmin, inmax, outmin, outmax);
    float w = remap(val.w, inmin, inmax, outmin, outmax);
    return float4(x, y, z, w);
}


namespace Nova
{
    public struct Light
    {
        public float intensity;
        public float3 color;
    };

    public struct DirectionalLight : Light
    {
        public float3 direction;
    };

    public struct AmbientLight : Light
    {

    };

    public struct PointLight : Light
    {
        public float innerRadius;
        public float outerRadius;
    };

    public struct Scene
    {
        public float4x4 modelMatrix;
        public float4x4 viewMatrix;
        public float4x4 projectionMatrix;
        public float3x3 normalMatrix;
        public float3 cameraWorldPos;
        public float dirLightIntensity;
        public float3 dirLightColor;
        public float3 dirLightDir;
        public float ambLightIntensity;
        public float3 ambLightColor;


        public float4 LocalToWorldPosition(float4 position)
        {
            return mul(modelMatrix, position);
        }

        public float4 LocalToViewPosition(float4 position)
        {
            return mul(mul(viewMatrix, modelMatrix), position);
        }

        public float4 LocalToClipPosition(float4 position)
        {
            return mul(mul(mul(projectionMatrix, viewMatrix), modelMatrix), position);
        }

        public float3 LocalToWorldNormal(float3 normal)
        {
            return mul(normalMatrix, normal);
        }
    };
}


