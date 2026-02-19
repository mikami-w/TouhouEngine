set(GAME_SOURCES
        BulletManager.cpp
        BulletManager.hpp
        Bullet.hpp
)

add_library(Game STATIC ${GAME_SOURCES})

set_target_properties(Game PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Game PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(Game
        PUBLIC ProjectPCH
        PUBLIC d3d11
)