set(GRAPHICS_SOURCES
        DX11Device.cpp
        DX11Device.hpp
)

add_library(Graphics STATIC ${GRAPHICS_SOURCES})

set_target_properties(Graphics PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Graphics PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(Graphics
        PUBLIC ProjectPCH
        PUBLIC d3d11 d3dcompiler dxgi
        PRIVATE Core
)