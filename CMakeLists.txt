cmake_minimum_required(VERSION 4.0)
project(TouhouEngine)

# ===== 链接时优化 (LTO/IPO) 设置 =====
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

if(supported)
    # 开启 Release 和 RelWithDebInfo 下的全局优化
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
    message(STATUS "LTO/IPO is enabled for Release and RelWithDebInfo configurations.")
else()
    message(WARNING "LTO/IPO is not supported by the current toolchain: ${error}")
endif()

# ===== C++ Standard Settings =====
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Output Directory =====
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # 设置可执行文件/DLL输出目录 (Windows DLL 会输出到这里)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # 设置静态库(.a/.lib)输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib) # 设置动态库(.so/.dylib)输出目录

# ===== Compiler Options =====
if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus /utf-8 /arch:AVX2)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    # 注意: -Wall -Wextra -Wpedantic 不要全局添加, 否则 SDL3 等第三方库也会产生大量警告
    # 严格警告只在 src/ 目录中对项目自身代码启用
    add_compile_options(-mavx2 -fopenmp-simd)
endif()

if(WIN32)
    # NOMINMAX / WIN32_LEAN_AND_MEAN: 减少 Windows.h 头文件污染
    # 注意: 不要全局定义 UNICODE/_UNICODE, 否则 SDL_main_impl.h 会生成 wWinMain 而非 WinMain
    # 导致与 MinGW CRT 启动代码不匹配. SDL3 内部自行处理 Unicode.
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
    if(MSVC)
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    endif()
endif()

# CMake 4.x 默认启用 C++ 模块扫描，需手动禁用
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# ===== SDL3 构建选项 (禁用不需要的组件) =====
set(SDL_GPU_SHADERCROSS OFF CACHE BOOL "" FORCE)  # 不需要运行时 shadercross, 使用离线编译
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)           # 禁用 HIDAPI (Clang 编译兼容性问题)

# ===== Build Targets =====
add_subdirectory(vendor)
add_subdirectory(src)
