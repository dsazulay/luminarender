
# Sources
file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_library(LuminaCore ${FILE_SOURCES})

target_link_directories(LuminaCore PUBLIC ../lib/assimp)

target_link_libraries(LuminaCore
    glad
    glfw
    glm
    fmt
    assimp
)

target_include_directories(LuminaCore
    PUBLIC ../include
)


