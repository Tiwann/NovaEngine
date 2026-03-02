module Common;
import Camera;
import Scene;

#ifdef NOVA_USE_GLSL_TYPEALISES
public typealias vec2 = float2;
public typealias vec3 = float3;
public typealias vec4 = float4;

public typealias ivec2 = int2;
public typealias ivec3 = int3;
public typealias ivec4 = int4;

public typealias uvec2 = uint2;
public typealias uvec3 = uint3;
public typealias uvec4 = uint4;

public typealias bvec2 = bool2;
public typealias bvec3 = bool3;
public typealias bvec4 = bool4;

public typealias mat2 = float2x2;
public typealias mat3 = float3x3;
public typealias mat4 = float4x4;

public typealias mat2x3 = float2x3;
public typealias mat2x4 = float2x4;

public typealias mat3x2 = float3x2;
public typealias mat3x4 = float3x4;

public typealias mat4x2 = float4x2;
public typealias mat4x3 = float4x3;

public typealias dvec2 = double2;
public typealias dvec3 = double3;
public typealias dvec4 = double4;

public typealias dmat2 = double2x2;
public typealias dmat3 = double3x3;
public typealias dmat4 = double4x4;

public typealias dmat2x3 = double2x3;
public typealias dmat2x4 = double2x4;

public typealias dmat3x2 = double3x2;
public typealias dmat3x4 = double3x4;

public typealias dmat4x2 = double4x2;
public typealias dmat4x3 = double4x3;
#endif

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

public float2x2 constructSpace(float2 i, float2 j)
{
    return {
        i.x, j.x,
        i.y, j.y,
    };
}

public float3x3 constructSpace(float3 i, float3 j, float3 k)
{
    return {
        i.x, j.x, k.x,
        i.y, j.y, k.y,
        i.z, j.z, k.z,
    };
}

public struct Contants<T> where T : __BuiltinFloatingPointType
{
	public static const T PI = (T)3.14159265359;
	public static const T TAU = T(2.0) * PI;
	public static const T Epsilon = T(0.00001);
}

// From Hazel Engine
float convertSRGBFromLinear(float linear)
{
	return linear <= 0.0031308f
		? linear * 12.92f
		: pow(linear, 1.0f / 2.4f) * 1.055f - 0.055f;
}

// From Hazel Engine, adapted
vector<T, N> convertSRGBToLinear<T, int N>(vector<T, N> srgb) where T : __BuiltinFloatingPointType
{
    vector<T, N> cutoff = vector<T, N>(0.04045);
    vector<T, N> a = vector<T, N>(0.055);
    vector<T, N> invGamma = vector<T, N>(2.4);
    vector<T, N> one = vector<T, N>(1.0);
    vector<T, N> invScale = vector<T, N>(12.92);

    vector<T, N> lower = srgb / invScale;
    vector<T, N> higher = pow((srgb + a) / (one + a), invGamma);

    vector<bool, N> mask = srgb < cutoff;

    return select(mask, lower, higher);
}