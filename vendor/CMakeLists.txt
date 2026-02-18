add_library(Vendor STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_impl.cpp
)

target_include_directories(Vendor PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
)