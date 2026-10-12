# 项目自身的编译选项 (不影响第三方库)
if(MSVC)
    # MSVC 严格警告已在根 CMakeLists.txt 全局设置
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    # _DEBUG 宏: GCC/Clang 不会自动定义, 需要手动为 Debug 配置添加
    add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)
endif()

# PCH
add_library(ProjectPCH INTERFACE)
target_precompile_headers(ProjectPCH INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/pch/pch.hpp"
)
target_include_directories(ProjectPCH INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/pch"
)

add_subdirectory(Core)
add_subdirectory(Graphics)
add_subdirectory(Game)
#add_subdirectory(Script)

add_executable(TouhouApp Engine_main.cpp)

set_target_properties(TouhouApp PROPERTIES LINKER_LANGUAGE CXX)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(WIN32)
        # 开启 WIN32 属性，不显示控制台窗口
        # SDL3::SDL3-main 提供 WinMain 入口, 无需手动指定 /ENTRY
        set_target_properties(TouhouApp PROPERTIES WIN32_EXECUTABLE TRUE)
    elseif(APPLE)
        # 开启 BUNDLE 属性，将其打包为 .app，双击运行时不会弹出 Terminal 终端
        set_target_properties(TouhouApp PROPERTIES MACOSX_BUNDLE TRUE)
    endif()
else()
    if(WIN32)
        set_target_properties(TouhouApp PROPERTIES WIN32_EXECUTABLE FALSE)
    elseif(APPLE)
        set_target_properties(TouhouApp PROPERTIES MACOSX_BUNDLE FALSE)
    endif()
endif()

target_link_libraries(TouhouApp PRIVATE
        ProjectPCH
        Core
        Graphics
        Game
        SDL3::SDL3
        #        Script
)

add_executable(ScriptCompiler ScriptCompiler_main.cpp)

set_target_properties(ScriptCompiler PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(ScriptCompiler PROPERTIES WIN32_EXECUTABLE FALSE)

target_link_libraries(ScriptCompiler PRIVATE
        ProjectPCH
        SDL3::SDL3
        #        Core
        #        Script
)