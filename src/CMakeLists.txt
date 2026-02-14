# PCH
add_library(ProjectPCH INTERFACE)
target_precompile_headers(ProjectPCH INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/pch/pch.hpp"
)
target_include_directories(ProjectPCH INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/pch"
)

add_subdirectory(Core)
#add_subdirectory(Graphics)
#add_subdirectory(Script)
#add_subdirectory(Game)

add_executable(TouhouApp Engine_main.cpp)

set_target_properties(TouhouApp PROPERTIES LINKER_LANGUAGE CXX)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    # Release 模式：开启 WIN32 属性，不显示控制台窗口
    set_target_properties(TouhouApp PROPERTIES WIN32_EXECUTABLE TRUE)
    # 强行指定 WIN32 程序入口点为 mainCRTStartup
    target_link_options(TouhouApp PRIVATE /ENTRY:mainCRTStartup)
else()
    # Debug 模式：不设置 WIN32，保留控制台窗口以便看日志
    set_target_properties(TouhouApp PROPERTIES WIN32_EXECUTABLE FALSE)
endif()

target_link_libraries(TouhouApp PRIVATE
        ProjectPCH
        Core
#        Graphics
#        Script
#        Game
)

add_executable(ScriptCompiler ScriptCompiler_main.cpp)

set_target_properties(ScriptCompiler PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(ScriptCompiler PROPERTIES WIN32_EXECUTABLE FALSE)

target_link_libraries(ScriptCompiler PRIVATE
        ProjectPCH
#        Core
#        Script
)