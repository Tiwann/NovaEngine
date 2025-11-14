module VertexData3D;

public struct VertexInput {
  public float3 position : POSITION;
  public float2 texCoord : TEXCOORDINATE;
  public float3 normal : NORMAL;
  public float4 color : COLOR;
};

public struct VertexOutput {
  public float4 position : SV_Position;
  public float2 texCoord;
  public float3 normal;
  public float4 color;
  public float3 fragPos;
};

public typedef VertexOutput FragmentInput;