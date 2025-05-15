module Common;

public typealias vec2 = float2;
public typealias vec3 = float3;
public typealias vec4 = float4;
public typealias mat2 = float2x2;
public typealias mat3 = float3x3;
public typealias mat4 = float4x4;

public vec2 operator*(mat2 lhs, vec2 rhs)
{
    return mul(lhs, rhs);
}

public vec3 operator*(mat3 lhs, vec3 rhs)
{
    return mul(lhs, rhs);
}

public vec4 operator*(mat4 lhs, vec4 rhs)
{
    return mul(lhs, rhs);
}

public vec3 operator*(mat3 lhs, vec4 rhs)
{
    return mul(lhs, rhs.xyz);
}


public vec4 operator*(mat4 lhs, vec3 rhs)
{
    return mul(lhs, vec4(rhs, 1.0));
}

public vec4 operator*(mat4 lhs, vec2 rhs)
{
    return mul(lhs, vec4(rhs, 0.0, 1.0));
}

public vec3 operator*(mat3 lhs, vec2 rhs)
{
    return mul(lhs, vec3(rhs, 1.0));
}

public float remap(float val, float inmin, float inmax, float outmin, float outmax)
{
    return outmin + (outmax - outmin) * ((val - inmin) / (inmax - inmin));
}

public vec2 remap(vec2 val, float inmin, float inmax, float outmin, float outmax)
{
    float x = remap(val.x, inmin, inmax, outmin, outmax);
    float y = remap(val.y, inmin, inmax, outmin, outmax);
    return vec2(x, y);
}

public vec3 remap(vec3 val, float inmin, float inmax, float outmin, float outmax)
{
    float x = remap(val.x, inmin, inmax, outmin, outmax);
    float y = remap(val.y, inmin, inmax, outmin, outmax);
    float z = remap(val.z, inmin, inmax, outmin, outmax);
    return vec3(x, y, z);
}

public vec4 remap(vec4 val, float inmin, float inmax, float outmin, float outmax)
{
    float x = remap(val.x, inmin, inmax, outmin, outmax);
    float y = remap(val.y, inmin, inmax, outmin, outmax);
    float z = remap(val.z, inmin, inmax, outmin, outmax);
    float w = remap(val.w, inmin, inmax, outmin, outmax);
    return vec4(x, y, z, w);
}


namespace Nova
{
    public struct Light
    {
        public float Intensity;
        public vec3 Color;
    };

    public struct DirectionalLight : Light
    {
        public vec3 Direction;
    };

    public struct AmbientLight : Light
    {

    };

    public struct PointLight : Light
    {
        public float InnerRadius;
        public float OuterRadius;
    };

    public struct Scene
    {
        public mat4 ModelMatrix;
        public mat4 ViewMatrix;
        public mat4 ProjectionMatrix;
        public mat3 NormalMatrix;
        public vec3 CameraViewDirection;
        public float DirLightIntensity;
        public vec3 DirLightColor;
        public vec3 DirLightDir;
        public float AmbLightIntensity;
        public vec3 AmbLightColor;


        public vec4 LocalToWorldPosition(vec4 Position)
        {
            return mul(ModelMatrix, Position);
        }

        public vec4 LocalToViewPosition(vec4 Position)
        {
            return mul(mul(ViewMatrix, ModelMatrix), Position);
        }

        public vec4 LocalToClipPosition(vec4 Position)
        {
            return mul(mul(mul(ProjectionMatrix, ViewMatrix), ModelMatrix), Position);
        }

        public vec3 LocalToWorldNormal(vec3 Normal)
        {
            return mul(NormalMatrix, Normal);
        }
    };
}


