set(GRAPHICS_SOURCES
        DX11Device.cpp
        DX11Device.hpp
        Shader.cpp
        Shader.hpp
        Vertex.hpp
        SpriteRenderer.cpp
        SpriteRenderer.hpp
)

add_library(Graphics STATIC ${GRAPHICS_SOURCES})

set_target_properties(Graphics PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Graphics PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(Graphics
        PUBLIC ProjectPCH
        PUBLIC d3d11 d3dcompiler dxgi
        PRIVATE Core
)

## 收集所有的 shader 文件, 用于在 IDE 中索引和管理 Shader
#file(GLOB_RECURSE SHADER_FILES "${CMAKE_SOURCE_DIR}/assets/shaders/*.hlsl")
#message(STATUS "Found shader files: ${SHADER_FILES}")
#add_custom_target(Shaders SOURCES ${SHADER_FILES})
#set_source_files_properties(${SHADER_FILES} PROPERTIES LANGUAGE HLSL)
