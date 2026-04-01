#pragma once

#include <array>
#include <cstdint>

/**
 * @brief D3D11 兼容颜色空间 (RGBA8, (DXGI_FORMAT_R8G8B8A8_UNORM))
 * 内存布局 (低地址 -> 高地址): [Red, Green, Blue, Alpha]
 * 对应 uint32_t 十六进制常量: 0xAABBGGRR
 */
namespace Graphics::Color {
using ColorType = std::uint32_t;
// 基础颜色 (RGBA 0xAABBGGRR)
enum : ColorType
{
  Black = 0xFF000000,
  Grey = 0xFF808080,
  DarkGrey = 0xFF505050,
  AshenGrey = 0xFF606060,
  White = 0xFFFFFFFF,
  TransparentWhite = 0x80FFFFFF,
  Red = 0xFF0000FF,
  Yellow = 0xFF00FFFF,
  LightRed = 0xFF8080FF,
  PastelRed = 0xFF6060FF,
  SunshineYellow = 0xFF40FFFF,
  Pink = 0xFFE0E0FF,
  LightCyan = 0xFFFFFFE0,
  Lavender = 0xFFFFE0E0,
  LightBlue = 0xFFFFD0D0,
  LightYellow = 0xFF80FFFF,
  PaleBlue = 0xFFFFB0C0,
  NeonBlue = 0xFFFF4040,
  DeepBlue = 0xFFFF3030,
  Gui1 = 0xFFFFF0E8,
  Gui2 = 0xFFF0E8FF,

  MenuActiveBackground = 0x40000000,
  StartMenuItemInactive = 0x80000030,
  KeyboardKeyHighlight = 0xC0FFFFFF,
  KeyboardKeyNormal = 0x60C0C0C0,
  MenuItemHighlight = 0x80FF0000,
  MenuItemDefault = 0x80502020,
  EndTextShadow = 0xFFD0D0C0, // 原 0xc0d0d0
  MusicRoomSongTitleText = 0xFFFFE0C0,
  MusicRoomSongTitleShadow = 0xFF802030,
  MusicRoomSongDescText = 0xFFC0E0FF,
  MusicRoomSongDescShadow = 0xFF000030,

  // --- 掩码与索引 ---
  RgbMask = 0x00FFFFFF,
  AlphaMask = 0xFF000000,

  // 对应 D3D11 内存索引: [0]=R, [1]=G, [2]=B, [3]=A
  RedIdx = 0,
  GreenIdx = 1,
  BlueIdx = 2,
  AlphaIdx = 3
};

/**
 * @brief 获取指定组件的值 (0-255)
 * @param idx 使用 Color::RedIdx 等
 */
constexpr std::uint8_t GetComponent(ColorType color, std::uint32_t idx) noexcept
{
  return static_cast<std::uint8_t>((color >> (idx * 8)) & 0xFF);
}

constexpr std::uint8_t R(ColorType c) noexcept
{
  return GetComponent(c, RedIdx);
}

constexpr std::uint8_t G(ColorType c) noexcept
{
  return GetComponent(c, GreenIdx);
}

constexpr std::uint8_t B(ColorType c) noexcept
{
  return GetComponent(c, BlueIdx);
}

constexpr std::uint8_t A(ColorType c) noexcept
{
  return GetComponent(c, AlphaIdx);
}

/**
 * @brief 修改 Alpha 通道并返回新颜色
 */
constexpr ColorType SetAlpha(ColorType color, std::uint8_t alpha) noexcept
{
  return (color & RgbMask) | (static_cast<std::uint32_t>(alpha) << 24);
}

/**
 * @brief 转换为 D3D11 Clear 函数使用的 float[4] 格式
 */
constexpr Core::Vector4 ToFloat4(ColorType c) noexcept
{
  return { R(c) / 255.0f, G(c) / 255.0f, B(c) / 255.0f, A(c) / 255.0f };
}
}
