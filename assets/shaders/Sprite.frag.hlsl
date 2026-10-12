// Sprite.frag.hlsl — HLSL SM6.0 Fragment Shader for SDL_GPU
Texture2D    uTexture : register(t0, space2);
SamplerState uSampler : register(s0, space2);

struct PSInput
{
  float4 position : SV_Position;
  float2 texCoord : TEXCOORD0;
  float4 color    : TEXCOORD1;
};

float4 main(PSInput input) : SV_Target0
{
  float4 texColor = uTexture.Sample(uSampler, input.texCoord);
  return texColor * input.color;
}
