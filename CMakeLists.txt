cmake_minimum_required(VERSION 4.0)
project(TouhouEngine)

# 检查是否使用 MSVC 编译器
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR
            "此项目仅支持 Microsoft Visual C++ (MSVC) 编译器.\n"
            "检测到的编译器: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER})\n"
    )
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

add_compile_options(/W4 /permissive- /Zc:__cplusplus /utf-8)
add_compile_definitions(UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS)

# ===== Build Targets =====

add_subdirectory(src)
add_subdirectory(vendor)