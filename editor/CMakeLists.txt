cmake_minimum_required(VERSION 3.21)
project(LuminaRenderEditor)

set(CMAKE_CXX_STANDARD 20)


# =============================================================

# Dependencies

# ImGui
message(STATUS "Installing imgui")
add_subdirectory(../external/imgui ../build/imgui)
set_property(TARGET ImGui PROPERTY FOLDER "Dependencies")

# ImGui
message(STATUS "Installing imguizmo")
add_subdirectory(../external/imguizmo ../build/imguizmo)
set_property(TARGET ImGuizmo PROPERTY FOLDER "Dependencies")


# LuminaRenderCore
message(STATUS "Installing luminarendercore")
add_subdirectory(../core ../build/core)
set_property(TARGET LuminaRenderCore PROPERTY FOLDER "Dependencies")

# =============================================================

# Sources
file(GLOB_RECURSE FILE_SOURCES RELATIVE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

add_executable(${PROJECT_NAME} ${FILE_SOURCES})

target_link_directories(${PROJECT_NAME} PUBLIC lib/assimp)

target_link_libraries(${PROJECT_NAME}
    Glad
    ImGui
    ImGuizmo
    glfw
    glm_static
    fmt
    assimp
    LuminaRenderCore
)

target_include_directories(${PROJECT_NAME}
    PUBLIC ../external/glfw
    PUBLIC ../external/glm
    PUBLIC ../external/fmt
    PUBLIC ../include
    PUBLIC ../core/src
)


