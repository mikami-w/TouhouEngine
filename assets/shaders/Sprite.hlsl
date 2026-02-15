// CPU 传给顶点着色器的数据结构
struct VSInput
{
  float3 position : POSITION; // 顶点位置 (x, y, z)
  float2 texCoord : TEXCOORD; // 纹理坐标 (u, v)
};

// 顶点着色器传给像素着色器的数据结构
struct PSInput
{
  float4 position : SV_POSITION; // 最终的屏幕空间位置 (x, y, z, w)
  float2 texCoord : TEXCOORD; // 纹理坐标 (u, v) (会被插值)
};

// 顶点着色器 (Vertex Shader)
PSInput VSMain(VSInput input)
{
  PSInput output;
  // 将 3D 坐标转为 4D 齐次坐标 (x, y, z, 1.0)
  output.position = float4(input.position, 1.0f);
  // 直接传递纹理坐标
  output.texCoord = input.texCoord;
  return output;
}

// 像素着色器 (Pixel Shader)
float4 PSMain(PSInput input) : SV_TARGET
{
  // 目前只输出一个固定颜色 (红色)，用于测试管线是否打通
  // 返回 RGBA 颜色 (红色不透明)
  // 等纹理模块做好了，这里会改成读取纹理的颜色
  return float4(1.0f, 0.0f, 0.0f, 1.0f);
}