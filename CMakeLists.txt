cmake_minimum_required(VERSION 3.21)
project(InteractiveGraphics)

set(CMAKE_CXX_STANDARD 20)

include_directories("include")
include_directories("src/imgui")
include_directories("/opt/homebrew/include")

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_executable(InteractiveGraphics ${FILE_SOURCES})

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(InteractiveGraphics PUBLIC glfw)

target_link_directories(InteractiveGraphics PUBLIC lib/assimp)
target_link_libraries(InteractiveGraphics PUBLIC assimp)

#target_precompile_headers(InteractiveGraphics PUBLIC pch.h)
