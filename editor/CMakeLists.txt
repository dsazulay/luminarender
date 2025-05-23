
# Sources
file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_executable(LuminaEditor ${FILE_SOURCES})

target_link_libraries(LuminaEditor
    ImGuizmo
    LuminaCore
)

target_include_directories(LuminaEditor
    PRIVATE ../core/src
)

