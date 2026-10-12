set(CORE_SOURCES
        Window.cpp
        Window.hpp
        Logger.hpp
        Timer.cpp
        Timer.hpp
        Application.cpp
        Application.hpp
        MathUtils.hpp
        XSRandom.hpp
        EngineSettings.hpp
        ConfigManager.cpp
        ConfigManager.hpp
        BasicTypes.hpp
        SAPHandleSystem.cpp
        SAPHandleSystem.hpp
)

add_library(Core STATIC ${CORE_SOURCES})

set_target_properties(Core PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Core PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(Core
        PUBLIC ProjectPCH
        PUBLIC SDL3::SDL3
        PRIVATE jsonpp
)