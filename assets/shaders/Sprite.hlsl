// 常量缓冲区
cbuffer TransformBuffer : register(b0)
{
  matrix projection;  // 投影矩阵, 负责把屏幕像素坐标投影到 NDC 坐标
};

// 纹理资源和采样器状态
Texture2D spriteTexture : register(t0); // 纹理资源绑定到 t0
SamplerState spriteSampler : register(s0); // 采样器状态绑定到 s0

// CPU 传给顶点着色器的数据结构
struct VSInput
{
  // 每顶点数据, 来自 Vertex Buffer (槽位0)
  float3 position : POSITION; // 顶点位置 (x, y, z)
  float2 texCoord : TEXCOORD; // 纹理坐标 (u, v)

  // 每实例数据, 来自 Instance Buffer (槽位1)
  float2 instPos    : INST_POS;   // 子弹位置 (x, y)
  float2 instScale  : INST_SCALE; // 子弹宽高 (scaleX, scaleY)
  float instRot     : INST_ROT;   // 子弹旋转弧度
  float4 instUvRect : INST_UV_RECT; // 纹理坐标矩形 (u0, v0, u1, v1) 用于从纹理图集中采样
  float4 instColor  : INST_COLOR; // 子弹颜色 (r, g, b, a)
};

// 顶点着色器传给像素着色器的数据结构
struct PSInput
{
  float4 position : SV_POSITION; // 最终的屏幕空间位置 (x, y, z, w)
  float2 texCoord : TEXCOORD; // 纹理坐标 (u, v) (会被插值)
  float4 color : COLOR; // 顶点颜色 (r, g, b, a)
};

// 顶点着色器 (Vertex Shader)
PSInput VSMain(VSInput input)
{
  PSInput output;
    
  // 获取基础的正方形顶点坐标 (-0.5 到 0.5)
  float2 pos = input.position.xy;
  
  // 缩放
  pos.x *= input.instScale.x;
  pos.y *= input.instScale.y;
  
  // 旋转 (在 GPU 上计算正弦余弦极其廉价)
  float s, c;
  sincos(input.instRot, s, c);
  float2 rotPos;
  rotPos.x = pos.x * c - pos.y * s;
  rotPos.y = pos.x * s + pos.y * c;
  
  // 平移, 到屏幕上的实际位置
  pos = rotPos + input.instPos;
  
  // 应用投影矩阵, 转换为 NDC 坐标
  float4 finalPos = float4(pos, 0.0f, 1.0f);
  output.position = mul(finalPos, projection);
  
  // 直接传递纹理坐标和实例颜色
  output.texCoord = input.texCoord * input.instUvRect.zw + input.instUvRect.xy; // 从纹理图集中采样: 基础坐标(0~1) * 抠图宽高 + 抠图左上角起点
  output.color = input.instColor;
  
  return output;
}

// 像素着色器 (Pixel Shader)
float4 PSMain(PSInput input) : SV_TARGET
{
  float4 texColor = spriteTexture.Sample(spriteSampler, input.texCoord);
    
  // 贴图颜色与实例颜色相乘, 实现变色/透明度控制
  return texColor * input.color; 
}