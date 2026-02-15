#include "Shader.hpp"
#include "Core/Logger.hpp"

#include <fstream>

namespace Graphics {

VertexShader::VertexShader(ID3D11Device* device, ID3DBlob* bytecode)
  : m_bytecode(bytecode)
{
  HRESULT hr = device->CreateVertexShader(
    bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Vertex Shader Object.");
}

void VertexShader::Bind(ID3D11DeviceContext* context)
{
  // 把顶点着色器装配到管线上
  context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
}

PixelShader::PixelShader(ID3D11Device* device, ID3DBlob* bytecode)
{
  HRESULT hr = device->CreatePixelShader(
    bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Pixel Shader Object.");
}

void PixelShader::Bind(ID3D11DeviceContext* context) const
{
  // 把像素着色器装配到管线上
  context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

InputLayout::InputLayout(ID3D11Device* device,
                         std::vector<D3D11_INPUT_ELEMENT_DESC> const& layoutDesc,
                         ID3DBlob* vsBytecode)
{
  // D3D11 使用定义的 C++ 结构描述去和编译好的 VS 字节码进行比对校验
  HRESULT hr = device->CreateInputLayout(layoutDesc.data(),
                                         layoutDesc.size(),
                                         vsBytecode->GetBufferPointer(),
                                         vsBytecode->GetBufferSize(),
                                         m_inputLayout.GetAddressOf());
  LOG_DX11_CHECK(hr, "Failed to create Input Layout.");
}

void InputLayout::Bind(ID3D11DeviceContext* context) const
{
  context->IASetInputLayout(m_inputLayout.Get());
}

Microsoft::WRL::ComPtr<ID3DBlob> ShaderCompiler::compile(std::string const& filename,
                                                         std::string const& entryPoint,
                                                         std::string const& profile)
{
  // 读取 HLSL 文件内容
  std::ifstream file(filename, std::ios::binary | std::ios::ate); // HLSL 文件
  if (!file.is_open()) {
    LOG_ERROR("Failed to open shader file: " + filename);
    throw std::runtime_error("Shader file not found");
  }
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  file.read(buffer.data(), size);

  Microsoft::WRL::ComPtr<ID3DBlob> bytecodeBlob;
  Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

  // 调用 D3DCompiler 编译 HLSL
  UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
  flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  HRESULT hr = D3DCompile(buffer.data(),               // 源代码数据
                          buffer.size(),               // 源代码数据
                          filename.c_str(),            // 用于报错的文件名
                          nullptr,                     // 宏和包含处理
                          nullptr,                     // 宏和包含处理
                          entryPoint.c_str(),          // 入口函数名
                          profile.c_str(),             // 目标配置
                          flags,                       // 编译标志
                          0,                           // 编译标志
                          bytecodeBlob.GetAddressOf(), // [输出] 成功后的字节码
                          errorBlob.GetAddressOf()     // [输出] 失败后的错误信息
  );

  if (FAILED(hr)) {
    if (errorBlob) {
      std::string errorMsg = static_cast<char*>(errorBlob->GetBufferPointer());
      LOG_ERROR(std::format("Shader compilation error in {}: \n{}", filename, errorMsg));
    } else {
      LOG_DX11_CHECK(hr,
                     std::format("Shader compilation error in {}: D3DCompile failed with unknown error.", filename));
    }
    throw std::runtime_error("Shader compilation failed");
  }

  return bytecodeBlob;
}
} // namespace Graphics
