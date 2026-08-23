add_library(jsonpp INTERFACE)

target_include_directories(jsonpp INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
)