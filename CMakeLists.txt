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
    add_compile_options(-Wall -Wextra -Wpedantic -mavx2 -fopenmp-simd)
endif()

if(WIN32)
    add_compile_definitions(UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    if(MSVC)
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    endif()
endif()

# ===== Build Targets =====
add_subdirectory(src)
add_subdirectory(vendor)
