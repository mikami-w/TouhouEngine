# compile_shaders.ps1 — 编译 HLSL SM6.0 着色器到 SPIR-V 和 DXIL
# 用法: 在项目根目录运行 powershell -ExecutionPolicy Bypass -File scripts/compile_shaders.ps1

$ErrorActionPreference = "Stop"

# ---- 路径配置 ----
$ProjectRoot = Split-Path -Parent $PSScriptRoot  # scripts/ 的上级 = 项目根
$ShaderSrcDir = Join-Path $ProjectRoot "assets\shaders"
$ShaderOutDir = Join-Path $ProjectRoot "assets\shaders\compiled"

# DXC 路径
$VulkanDxc = "D:\libs\VulkanSDK\1.4.350.0\Bin\dxc.exe"       # SPIR-V
$WinSdkDxc = "C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\dxc.exe"  # DXIL

# ---- 验证工具 ----
if (!(Test-Path $VulkanDxc)) {
    Write-Error "Vulkan DXC not found: $VulkanDxc"
    exit 1
}
if (!(Test-Path $WinSdkDxc)) {
    Write-Error "Windows SDK DXC not found: $WinSdkDxc"
    exit 1
}

# 创建输出目录
New-Item -ItemType Directory -Force -Path $ShaderOutDir | Out-Null

# ---- 着色器列表 ----
$Shaders = @(
    @{ Name = "Sprite.vert"; Profile = "vs_6_0" },
    @{ Name = "Sprite.frag"; Profile = "ps_6_0" }
)

$failed = $false

foreach ($shader in $Shaders) {
    $src = Join-Path $ShaderSrcDir "$($shader.Name).hlsl"
    $profile = $shader.Profile

    if (!(Test-Path $src)) {
        Write-Error "Source not found: $src"
        $failed = $true
        continue
    }

    Write-Host "Compiling $($shader.Name) ($profile)..."

    # --- SPIR-V (Vulkan) ---
    $spvOut = Join-Path $ShaderOutDir "$($shader.Name).spv"
    Write-Host "  -> SPIR-V: $spvOut"
    & $VulkanDxc -spirv -T $profile -E main -Fo $spvOut $src 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Error "  SPIR-V compilation FAILED for $($shader.Name)"
        $failed = $true
    }

    # --- DXIL (D3D12) ---
    $dxilOut = Join-Path $ShaderOutDir "$($shader.Name).dxil"
    Write-Host "  -> DXIL:   $dxilOut"
    & $WinSdkDxc -T $profile -E main -Fo $dxilOut $src 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Error "  DXIL compilation FAILED for $($shader.Name)"
        $failed = $true
    }
}

if ($failed) {
    Write-Error "Some shaders failed to compile!"
    exit 1
}

Write-Host ""
Write-Host "All shaders compiled successfully to: $ShaderOutDir"
Get-ChildItem $ShaderOutDir | Format-Table Name, Length -AutoSize
