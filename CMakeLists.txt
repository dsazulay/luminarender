cmake_minimum_required(VERSION 3.21)
project(InteractiveGraphics)

set(CMAKE_CXX_STANDARD 20)

include_directories("include")
include_directories("src/imgui")
include_directories("/opt/homebrew/include")

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

# =============================================================

# Dependencies

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "GLFW lib only" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only" FORCE)
message(STATUS "Installing glfw via submodule")
add_subdirectory(external/glfw)
set_property(TARGET glfw PROPERTY FOLDER "Dependencies")

# fmt
message(STATUS "Installing fmt via submodule")
add_subdirectory(external/fmt)
set_property(TARGET fmt PROPERTY FOLDER "Dependencies")

# =============================================================

# Sources
file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_executable(${PROJECT_NAME} ${FILE_SOURCES})

target_link_libraries(${PROJECT_NAME}
    PUBLIC glfw
    PUBLIC fmt
)

target_include_directories(${PROJECT_NAME}
    PUBLIC external/glfw
    PUBLIC external/fmt
)

target_link_directories(InteractiveGraphics PUBLIC lib/assimp)
target_link_libraries(InteractiveGraphics PUBLIC assimp)

