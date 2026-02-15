#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>
#include <wrl/client.h>

namespace Graphics {
// 顶点着色器
class VertexShader
{
public:
  VertexShader(ID3D11Device* device, ID3DBlob* bytecode); // 构造函数接收编译后的字节码 Blob
  void Bind(ID3D11DeviceContext* context);          // 绑定到管线
  ID3DBlob* getBytecode() const { return m_bytecode.Get(); }

private:
  Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
  Microsoft::WRL::ComPtr<ID3DBlob> m_bytecode;
};

// 像素着色器
class PixelShader
{
public:
  PixelShader(ID3D11Device* device, ID3DBlob* bytecode);
  void Bind(ID3D11DeviceContext* context) const;

private:
  Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
};

// 输入布局, 定义顶点数据的格式和如何绑定到顶点着色器的输入
class InputLayout
{
public:
  InputLayout(ID3D11Device* device, std::vector<D3D11_INPUT_ELEMENT_DESC> const& layoutDesc, ID3DBlob* vsBytecode);
  void Bind(ID3D11DeviceContext* context) const;

private:
  Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
};

class ShaderCompiler
{
public:
  static Microsoft::WRL::ComPtr<ID3DBlob> compile(std::string const& filename,
                                                  std::string const& entryPoint,
                                                  std::string const& profile);
};
} // namespace Graphics
