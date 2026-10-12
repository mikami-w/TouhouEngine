// Sprite.vert.hlsl — HLSL SM6.0 Vertex Shader for SDL_GPU
// 顶点 Uniform (投影矩阵) — register(b0, space1)
cbuffer ProjectionUB : register(b0, space1)
{
  // CPU uploads this matrix in row-major order.
  row_major float4x4 uProjection;
};

struct VSInput
{
  // 每顶点 (槽位 0) — SDL3 使用 TEXCOORD 语义名 + location 索引
  float3 position : TEXCOORD0; // location 0
  float2 texCoord : TEXCOORD1; // location 1

  // 每实例 (槽位 1)
  float2 instPos    : TEXCOORD2; // location 2
  float2 instScale  : TEXCOORD3; // location 3
  float  instRot    : TEXCOORD4; // location 4
  float4 instUvRect : TEXCOORD5; // location 5
  float4 instColor  : TEXCOORD6; // location 6
};

struct VSOutput
{
  float4 position : SV_Position;
  float2 texCoord : TEXCOORD0;
  float4 color    : TEXCOORD1;
};

VSOutput main(VSInput input)
{
  VSOutput output;

  // 获取基础的正方形顶点坐标 (-0.5 到 0.5)
  float2 pos = input.position.xy;

  // 缩放
  pos.x *= input.instScale.x;
  pos.y *= input.instScale.y;

  // 旋转
  float s, c;
  sincos(input.instRot, s, c);
  float2 rotPos;
  rotPos.x = pos.x * c - pos.y * s;
  rotPos.y = pos.x * s + pos.y * c;

  // 平移
  pos = rotPos + input.instPos;

  // 应用投影矩阵
  float4 finalPos = float4(pos, 0.0f, 1.0f);
  output.position = mul(finalPos, uProjection);

  // 纹理坐标从图集中采样
  output.texCoord = input.texCoord * input.instUvRect.zw + input.instUvRect.xy;
  output.color = input.instColor;

  return output;
}
