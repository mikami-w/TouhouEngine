set(GAME_SOURCES
        Bullet.hpp
        BulletManager.cpp
        BulletManager.hpp
        BulletEmitter.cpp
        BulletEmitter.hpp
        World.cpp
        World.hpp
)

add_library(Game STATIC ${GAME_SOURCES})

set_target_properties(Game PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(Game PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(Game
        PUBLIC ProjectPCH
        PUBLIC d3d11
)