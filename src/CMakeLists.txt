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
        set_target_properties(TouhouApp PROPERTIES WIN32_EXECUTABLE TRUE)
        # 强行指定 WIN32 程序入口点为 mainCRTStartup (仅 MSVC)
        # TODO: 移除 DirectX 依赖后删除该编译选项
        if(MSVC)
            target_link_options(TouhouApp PRIVATE /ENTRY:mainCRTStartup)
        endif ()
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
        #        Script
)

add_executable(ScriptCompiler ScriptCompiler_main.cpp)

set_target_properties(ScriptCompiler PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(ScriptCompiler PROPERTIES WIN32_EXECUTABLE FALSE)

target_link_libraries(ScriptCompiler PRIVATE
        ProjectPCH
        #        Core
        #        Script
)